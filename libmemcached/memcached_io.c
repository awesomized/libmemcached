/*
  Basic socket buffered IO
*/

#include "common.h"
#include "memcached_io.h"
#include <sys/select.h>
#include <poll.h>

typedef enum {
  MEM_READ,
  MEM_WRITE
} memc_read_or_write;

static ssize_t io_flush(memcached_server_st *ptr, memcached_return *error);
static void increment_udp_message_id(memcached_server_st *ptr);

static memcached_return io_wait(memcached_server_st *ptr,
                                memc_read_or_write read_or_write)
{
  struct pollfd fds= {
     .fd= ptr->fd,
     .events = POLLIN
  };
  int error;

  unlikely (read_or_write == MEM_WRITE) /* write */
    fds.events= POLLOUT;

  /*
  ** We are going to block on write, but at least on Solaris we might block
  ** on write if we haven't read anything from our input buffer..
  ** Try to purge the input buffer if we don't do any flow control in the
  ** application layer (just sending a lot of data etc)
  ** The test is moved down in the purge function to avoid duplication of
  ** the test.
  */
  if (read_or_write == MEM_WRITE)
  {
    memcached_return rc= memcached_purge(ptr);
    if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_STORED)
      return MEMCACHED_FAILURE;
  }

  int timeout= ptr->root->poll_timeout;
  if ((ptr->root->flags & MEM_NO_BLOCK) == 0)
    timeout= -1;

  error= poll(&fds, 1, timeout);

  if (error == 1)
    return MEMCACHED_SUCCESS;
  else if (error == 0)
    return MEMCACHED_TIMEOUT;

  /* Imposssible for anything other then -1 */
  WATCHPOINT_ASSERT(error == -1);
  memcached_quit_server(ptr, 1);

  return MEMCACHED_FAILURE;
}

/**
 * Try to fill the input buffer for a server with as much
 * data as possible.
 *
 * @param ptr the server to pack
 */
static bool repack_input_buffer(memcached_server_st *ptr)
{
  if (ptr->read_ptr != ptr->read_buffer)
  {
    /* Move all of the data to the beginning of the buffer so
    ** that we can fit more data into the buffer...
    */
    memmove(ptr->read_buffer, ptr->read_ptr, ptr->read_buffer_length);
    ptr->read_ptr= ptr->read_buffer;
    ptr->read_data_length= ptr->read_buffer_length;
  }

  /* There is room in the buffer, try to fill it! */
  if (ptr->read_buffer_length != MEMCACHED_MAX_BUFFER)
  {
    /* Just try a single read to grab what's available */
    ssize_t nr= read(ptr->fd,
                     ptr->read_ptr + ptr->read_data_length,
                     MEMCACHED_MAX_BUFFER - ptr->read_data_length);

    if (nr > 0)
    {
      ptr->read_data_length+= (size_t)nr;
      ptr->read_buffer_length+= (size_t)nr;
      return true;
    }
  }
  return false;
}

/**
 * If the we have callbacks connected to this server structure
 * we may start process the input queue and fire the callbacks
 * for the incomming messages. This function is _only_ called
 * when the input buffer is full, so that we _know_ that we have
 * at least _one_ message to process.
 *
 * @param ptr the server to star processing iput messages for
 * @return true if we processed anything, false otherwise
 */
static bool process_input_buffer(memcached_server_st *ptr)
{
  /*
  ** We might be able to process some of the response messages if we
  ** have a callback set up
  */
  if (ptr->root->callbacks != NULL && (ptr->root->flags & MEM_USE_UDP) == 0)
  {
    /*
     * We might have responses... try to read them out and fire
     * callbacks
     */
    memcached_callback_st cb= *ptr->root->callbacks;

    char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
    memcached_return error;
    error= memcached_response(ptr, buffer, sizeof(buffer),
                              &ptr->root->result);
    if (error == MEMCACHED_SUCCESS)
    {
      for (unsigned int x= 0; x < cb.number_of_callback; x++)
      {
        error= (*cb.callback[x])(ptr->root, &ptr->root->result, cb.context);
        if (error != MEMCACHED_SUCCESS)
          break;
      }

      /* @todo what should I do with the error message??? */
    }
    /* @todo what should I do with other error messages?? */
    return true;
  }

  return false;
}

#ifdef UNUSED
void memcached_io_preread(memcached_st *ptr)
{
  unsigned int x;

  return;

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (memcached_server_response_count(ptr, x) &&
        ptr->hosts[x].read_data_length < MEMCACHED_MAX_BUFFER )
    {
      size_t data_read;

      data_read= read(ptr->hosts[x].fd,
                      ptr->hosts[x].read_ptr + ptr->hosts[x].read_data_length,
                      MEMCACHED_MAX_BUFFER - ptr->hosts[x].read_data_length);
      if (data_read == -1)
        continue;

      ptr->hosts[x].read_buffer_length+= data_read;
      ptr->hosts[x].read_data_length+= data_read;
    }
  }
}
#endif

memcached_return memcached_io_read(memcached_server_st *ptr,
                                   void *buffer, size_t length, ssize_t *nread)
{
  char *buffer_ptr;

  buffer_ptr= buffer;

  while (length)
  {
    if (!ptr->read_buffer_length)
    {
      ssize_t data_read;

      while (1)
      {
        data_read= read(ptr->fd, ptr->read_buffer, MEMCACHED_MAX_BUFFER);
        if (data_read > 0)
          break;
        else if (data_read == -1)
        {
          ptr->cached_errno= errno;
          memcached_return rc= MEMCACHED_UNKNOWN_READ_FAILURE;
          switch (errno)
          {
          case EAGAIN:
          case EINTR:
            if ((rc= io_wait(ptr, MEM_READ)) == MEMCACHED_SUCCESS)
              continue;
          /* fall through */

          default:
            {
              memcached_quit_server(ptr, 1);
              *nread= -1;
              return rc;
            }
          }
        }
        else
        {
          /*
            EOF. Any data received so far is incomplete
            so discard it. This always reads by byte in case of TCP
            and protocol enforcement happens at memcached_response()
            looking for '\n'. We do not care for UDB which requests 8 bytes
            at once. Generally, this means that connection went away. Since
            for blocking I/O we do not return 0 and for non-blocking case
            it will return EGAIN if data is not immediatly available.
          */
          memcached_quit_server(ptr, 1);
          *nread= -1;
          return MEMCACHED_UNKNOWN_READ_FAILURE;
        }
      }

      ptr->io_bytes_sent = 0;
      ptr->read_data_length= (size_t) data_read;
      ptr->read_buffer_length= (size_t) data_read;
      ptr->read_ptr= ptr->read_buffer;
    }

    if (length > 1)
    {
      size_t difference;

      difference= (length > ptr->read_buffer_length) ? ptr->read_buffer_length : length;

      memcpy(buffer_ptr, ptr->read_ptr, difference);
      length -= difference;
      ptr->read_ptr+= difference;
      ptr->read_buffer_length-= difference;
      buffer_ptr+= difference;
    }
    else
    {
      *buffer_ptr= *ptr->read_ptr;
      ptr->read_ptr++;
      ptr->read_buffer_length--;
      buffer_ptr++;
      break;
    }
  }

  ptr->server_failure_counter= 0;
  *nread = (ssize_t)(buffer_ptr - (char*)buffer);
  return MEMCACHED_SUCCESS;
}

ssize_t memcached_io_write(memcached_server_st *ptr,
                           const void *buffer, size_t length, char with_flush)
{
  size_t original_length;
  const char* buffer_ptr;

  WATCHPOINT_ASSERT(ptr->fd != -1);

  original_length= length;
  buffer_ptr= buffer;

  while (length)
  {
    char *write_ptr;
    size_t should_write;
    size_t buffer_end;

    if (ptr->type == MEMCACHED_CONNECTION_UDP)
    {
      //UDP does not support partial writes
      buffer_end= MAX_UDP_DATAGRAM_LENGTH;
      should_write= length;
      if (ptr->write_buffer_offset + should_write > buffer_end)
        return -1;
    }
    else
    {
      buffer_end= MEMCACHED_MAX_BUFFER;
      should_write= buffer_end - ptr->write_buffer_offset;
      should_write= (should_write < length) ? should_write : length;
    }

    write_ptr= ptr->write_buffer + ptr->write_buffer_offset;
    memcpy(write_ptr, buffer_ptr, should_write);
    ptr->write_buffer_offset+= should_write;
    buffer_ptr+= should_write;
    length-= should_write;

    if (ptr->write_buffer_offset == buffer_end && ptr->type != MEMCACHED_CONNECTION_UDP)
    {
      memcached_return rc;
      ssize_t sent_length;

      WATCHPOINT_ASSERT(ptr->fd != -1);
      sent_length= io_flush(ptr, &rc);
      if (sent_length == -1)
        return -1;

      /* If io_flush calls memcached_purge, sent_length may be 0 */
      unlikely (sent_length != 0)
      {
        WATCHPOINT_ASSERT(sent_length == (ssize_t)buffer_end);
      }
    }
  }

  if (with_flush)
  {
    memcached_return rc;
    WATCHPOINT_ASSERT(ptr->fd != -1);
    if (io_flush(ptr, &rc) == -1)
      return -1;
  }

  return (ssize_t) original_length;
}

memcached_return memcached_io_close(memcached_server_st *ptr)
{
  int r;

  if (ptr->fd == -1)
    return MEMCACHED_SUCCESS;

  /* in case of death shutdown to avoid blocking at close() */
  if (1)
  {
    r= shutdown(ptr->fd, SHUT_RDWR);

#ifdef DEBUG
    if (r && errno != ENOTCONN)
    {
      WATCHPOINT_NUMBER(ptr->fd);
      WATCHPOINT_ERRNO(errno);
      WATCHPOINT_ASSERT(errno);
    }
#endif
  }

  r= close(ptr->fd);
#ifdef DEBUG
  if (r != 0)
    WATCHPOINT_ERRNO(errno);
#endif

  return MEMCACHED_SUCCESS;
}

memcached_server_st *memcached_io_get_readable_server(memcached_st *memc)
{
#define MAX_SERVERS_TO_POLL 100
  struct pollfd fds[MAX_SERVERS_TO_POLL];
  unsigned int host_index= 0;

  for (unsigned int x= 0;
       x< memc->number_of_hosts && host_index < MAX_SERVERS_TO_POLL;
       ++x)
  {
    if (memc->hosts[x].read_buffer_length > 0) /* I have data in the buffer */
      return &memc->hosts[x];

    if (memcached_server_response_count(&memc->hosts[x]) > 0)
    {
      fds[host_index].events = POLLIN;
      fds[host_index].revents = 0;
      fds[host_index].fd = memc->hosts[x].fd;
      ++host_index;
    }
  }

  if (host_index < 2)
  {
    /* We have 0 or 1 server with pending events.. */
    for (unsigned int x= 0; x< memc->number_of_hosts; ++x)
      if (memcached_server_response_count(&memc->hosts[x]) > 0)
        return &memc->hosts[x];

    return NULL;
  }

  int err= poll(fds, host_index, memc->poll_timeout);
  switch (err) {
  case -1:
    memc->cached_errno = errno;
    /* FALLTHROUGH */
  case 0:
    break;
  default:
    for (unsigned int x= 0; x < host_index; ++x)
      if (fds[x].revents & POLLIN)
        for (unsigned int y= 0; y < memc->number_of_hosts; ++y)
          if (memc->hosts[y].fd == fds[x].fd)
            return &memc->hosts[y];
  }

  return NULL;
}

static ssize_t io_flush(memcached_server_st *ptr,
                        memcached_return *error)
{
  /*
  ** We might want to purge the input buffer if we haven't consumed
  ** any output yet... The test for the limits is the purge is inline
  ** in the purge function to avoid duplicating the logic..
  */
  {
     memcached_return rc;
     WATCHPOINT_ASSERT(ptr->fd != -1);
     rc= memcached_purge(ptr);

     if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_STORED)
       return -1;
  }
  ssize_t sent_length;
  size_t return_length;
  char *local_write_ptr= ptr->write_buffer;
  size_t write_length= ptr->write_buffer_offset;

  *error= MEMCACHED_SUCCESS;

  WATCHPOINT_ASSERT(ptr->fd != -1);

  // UDP Sanity check, make sure that we are not sending somthing too big
  if (ptr->type == MEMCACHED_CONNECTION_UDP && write_length > MAX_UDP_DATAGRAM_LENGTH)
    return -1;

  if (ptr->write_buffer_offset == 0 || (ptr->type == MEMCACHED_CONNECTION_UDP
          && ptr->write_buffer_offset == UDP_DATAGRAM_HEADER_LENGTH))
    return 0;

  /* Looking for memory overflows */
#if defined(DEBUG)
  if (write_length == MEMCACHED_MAX_BUFFER)
    WATCHPOINT_ASSERT(ptr->write_buffer == local_write_ptr);
  WATCHPOINT_ASSERT((ptr->write_buffer + MEMCACHED_MAX_BUFFER) >= (local_write_ptr + write_length));
#endif

  return_length= 0;
  while (write_length)
  {
    WATCHPOINT_ASSERT(ptr->fd != -1);
    WATCHPOINT_ASSERT(write_length > 0);
    sent_length= 0;
    if (ptr->type == MEMCACHED_CONNECTION_UDP)
      increment_udp_message_id(ptr);
    sent_length= write(ptr->fd, local_write_ptr, write_length);

    if (sent_length == -1)
    {
      ptr->cached_errno= errno;
      switch (errno)
      {
      case ENOBUFS:
        continue;
      case EAGAIN:
      {
        /*
         * We may be blocked on write because the input buffer
         * is full. Let's check if we have room in our input
         * buffer for more data and retry the write before
         * waiting..
         */
        if (repack_input_buffer(ptr) ||
            process_input_buffer(ptr))
          continue;

        memcached_return rc;
        rc= io_wait(ptr, MEM_WRITE);

        if (rc == MEMCACHED_SUCCESS || rc == MEMCACHED_TIMEOUT)
          continue;

        memcached_quit_server(ptr, 1);
        return -1;
      }
      default:
        memcached_quit_server(ptr, 1);
        *error= MEMCACHED_ERRNO;
        return -1;
      }
    }

    if (ptr->type == MEMCACHED_CONNECTION_UDP &&
        (size_t)sent_length != write_length)
    {
      memcached_quit_server(ptr, 1);
      return -1;
    }

    ptr->io_bytes_sent += (uint32_t) sent_length;

    local_write_ptr+= sent_length;
    write_length-= (uint32_t) sent_length;
    return_length+= (uint32_t) sent_length;
  }

  WATCHPOINT_ASSERT(write_length == 0);
  // Need to study this assert() WATCHPOINT_ASSERT(return_length ==
  // ptr->write_buffer_offset);

  // if we are a udp server, the begining of the buffer is reserverd for
  // the upd frame header
  if (ptr->type == MEMCACHED_CONNECTION_UDP)
    ptr->write_buffer_offset= UDP_DATAGRAM_HEADER_LENGTH;
  else
    ptr->write_buffer_offset= 0;

  return (ssize_t) return_length;
}

/*
  Eventually we will just kill off the server with the problem.
*/
void memcached_io_reset(memcached_server_st *ptr)
{
  memcached_quit_server(ptr, 1);
}

/**
 * Read a given number of bytes from the server and place it into a specific
 * buffer. Reset the IO channel on this server if an error occurs.
 */
memcached_return memcached_safe_read(memcached_server_st *ptr,
                                     void *dta,
                                     size_t size)
{
  size_t offset= 0;
  char *data= dta;

  while (offset < size)
  {
    ssize_t nread;
    memcached_return rc= memcached_io_read(ptr, data + offset, size - offset,
                                           &nread);
    if (rc != MEMCACHED_SUCCESS)
      return rc;

    offset+= (size_t) nread;
  }

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_io_readline(memcached_server_st *ptr,
                                       char *buffer_ptr,
                                       size_t size)
{
  bool line_complete= false;
  size_t total_nr= 0;

  while (!line_complete)
  {
    if (ptr->read_buffer_length == 0)
    {
      /*
       * We don't have any data in the buffer, so let's fill the read
       * buffer. Call the standard read function to avoid duplicating
       * the logic.
       */
      ssize_t nread;
      memcached_return rc= memcached_io_read(ptr, buffer_ptr, 1, &nread);
      if (rc != MEMCACHED_SUCCESS)
        return rc;

      if (*buffer_ptr == '\n')
        line_complete= true;

      ++buffer_ptr;
      ++total_nr;
    }

    /* Now let's look in the buffer and copy as we go! */
    while (ptr->read_buffer_length && total_nr < size && !line_complete)
    {
      *buffer_ptr = *ptr->read_ptr;
      if (*buffer_ptr == '\n')
        line_complete = true;
      --ptr->read_buffer_length;
      ++ptr->read_ptr;
      ++total_nr;
      ++buffer_ptr;
    }

    if (total_nr == size)
      return MEMCACHED_PROTOCOL_ERROR;
  }

  return MEMCACHED_SUCCESS;
}

/*
 * The udp request id consists of two seperate sections
 *   1) The thread id
 *   2) The message number
 * The thread id should only be set when the memcached_st struct is created
 * and should not be changed.
 *
 * The message num is incremented for each new message we send, this function
 * extracts the message number from message_id, increments it and then
 * writes the new value back into the header
 */
static void increment_udp_message_id(memcached_server_st *ptr)
{
  struct udp_datagram_header_st *header= (struct udp_datagram_header_st *)ptr->write_buffer;
  uint16_t cur_req= get_udp_datagram_request_id(header);
  int msg_num= get_msg_num_from_request_id(cur_req);
  int thread_id= get_thread_id_from_request_id(cur_req);

  if (((++msg_num) & UDP_REQUEST_ID_THREAD_MASK) != 0)
    msg_num= 0;

  header->request_id= htons((uint16_t) (thread_id | msg_num));
}

memcached_return memcached_io_init_udp_header(memcached_server_st *ptr, uint16_t thread_id)
{
  if (thread_id > UDP_REQUEST_ID_MAX_THREAD_ID)
    return MEMCACHED_FAILURE;

  struct udp_datagram_header_st *header= (struct udp_datagram_header_st *)ptr->write_buffer;
  header->request_id= htons((uint16_t) (generate_udp_request_thread_id(thread_id)));
  header->num_datagrams= htons(1);
  header->sequence_number= htons(0);

  return MEMCACHED_SUCCESS;
}
