/*
  Basic socket buffered IO
*/

#include "common.h"
#include "memcached_io.h"
#include <sys/select.h>
#include <poll.h>

typedef enum {
  MEM_READ,
  MEM_WRITE,
} memc_read_or_write;

static ssize_t io_flush(memcached_server_st *ptr, memcached_return *error);

static memcached_return io_wait(memcached_server_st *ptr,
                                memc_read_or_write read_or_write)
{
  struct pollfd fds[1];
  short flags= 0;
  int error;

  if (read_or_write == MEM_WRITE) /* write */
    flags= POLLOUT |  POLLERR;
  else
    flags= POLLIN | POLLERR;

  memset(&fds, 0, sizeof(struct pollfd));
  fds[0].fd= ptr->fd;
  fds[0].events= flags;

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
    memcached_return rc=memcached_purge(ptr);
    if (rc != MEMCACHED_SUCCESS && rc != MEMCACHED_STORED)
       return MEMCACHED_FAILURE;
  }

  error= poll(fds, 1, ptr->root->poll_timeout);

  if (error == 1)
    return MEMCACHED_SUCCESS;
  else if (error == 0)
  {
    return MEMCACHED_TIMEOUT;
  }

  /* Imposssible for anything other then -1 */
  WATCHPOINT_ASSERT(error == -1);
  memcached_quit_server(ptr, 1);

  return MEMCACHED_FAILURE;

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

ssize_t memcached_io_read(memcached_server_st *ptr,
                          void *buffer, size_t length)
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
        data_read= read(ptr->fd, 
                        ptr->read_buffer, 
                        MEMCACHED_MAX_BUFFER);
        if (data_read > 0)
          break;
        else if (data_read == -1)
        {
          ptr->cached_errno= errno;
          switch (errno)
          {
          case EAGAIN:
          case EINTR: 
            {
              memcached_return rc;

              rc= io_wait(ptr, MEM_READ);

              if (rc == MEMCACHED_SUCCESS)
                continue;
            }
          /* fall trough */
          default:
            {
              memcached_quit_server(ptr, 1);
              return -1;
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
          return -1;
        }
      }

      ptr->io_bytes_sent = 0;
      ptr->read_data_length= data_read;
      ptr->read_buffer_length= data_read;
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

  return (size_t)(buffer_ptr - (char*)buffer);
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

    should_write= MEMCACHED_MAX_BUFFER - ptr->write_buffer_offset;
    write_ptr= ptr->write_buffer + ptr->write_buffer_offset;

    should_write= (should_write < length) ? should_write : length;

    memcpy(write_ptr, buffer_ptr, should_write);
    ptr->write_buffer_offset+= should_write;
    buffer_ptr+= should_write;
    length-= should_write;

    if (ptr->write_buffer_offset == MEMCACHED_MAX_BUFFER)
    {
      memcached_return rc;
      ssize_t sent_length;

      WATCHPOINT_ASSERT(ptr->fd != -1);
      sent_length= io_flush(ptr, &rc);
      if (sent_length == -1)
        return -1;

      /* If io_flush calls memcached_purge, sent_length may be 0 */
      if (sent_length != 0)
        WATCHPOINT_ASSERT(sent_length == MEMCACHED_MAX_BUFFER);
    }
  }

  if (with_flush)
  {
    memcached_return rc;
    WATCHPOINT_ASSERT(ptr->fd != -1);
    if (io_flush(ptr, &rc) == -1)
      return -1;
  }

  return original_length;
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

#ifdef HAVE_DEBUG
    if (r && errno != ENOTCONN)
    {
      WATCHPOINT_NUMBER(ptr->fd);
      WATCHPOINT_ERRNO(errno);
      WATCHPOINT_ASSERT(errno);
    }
#endif
  }

  r= close(ptr->fd);
#ifdef HAVE_DEBUG
  if (r != 0)
    WATCHPOINT_ERRNO(errno);
#endif

  return MEMCACHED_SUCCESS;
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

  if (ptr->write_buffer_offset == 0)
    return 0;

  /* Looking for memory overflows */
#if defined(HAVE_DEBUG)
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
    {
      struct addrinfo *ai;

      ai= ptr->address_info;

      /* Crappy test code */
      char buffer[HUGE_STRING_LEN + 8];
      memset(buffer, 0, HUGE_STRING_LEN + 8);
      memcpy (buffer+8, local_write_ptr, write_length);
      buffer[0]= 0;
      buffer[1]= 0;
      buffer[2]= 0;
      buffer[3]= 0;
      buffer[4]= 0;
      buffer[5]= 1;
      buffer[6]= 0;
      buffer[7]= 0;
      sent_length= sendto(ptr->fd, buffer, write_length + 8, 0, 
                          (struct sockaddr *)ai->ai_addr, 
                          ai->ai_addrlen);
      if (sent_length == -1)
      {
        WATCHPOINT_ERRNO(errno);
        WATCHPOINT_ASSERT(0);
      }
      sent_length-= 8; /* We remove the header */
    }
    else
    {
      WATCHPOINT_ASSERT(ptr->fd != -1);
      if ((sent_length= write(ptr->fd, local_write_ptr, 
                              write_length)) == -1)
      {
        switch (errno)
        {
        case ENOBUFS:
          continue;
        case EAGAIN:
          {
            memcached_return rc;
            rc= io_wait(ptr, MEM_WRITE);

            if (rc == MEMCACHED_SUCCESS || rc == MEMCACHED_TIMEOUT) 
              continue;

            memcached_quit_server(ptr, 1);
            return -1;
          }
        default:
          memcached_quit_server(ptr, 1);
          ptr->cached_errno= errno;
          *error= MEMCACHED_ERRNO;
          return -1;
        }
      }
    }

    ptr->io_bytes_sent += sent_length;

    local_write_ptr+= sent_length;
    write_length-= sent_length;
    return_length+= sent_length;
  }

  WATCHPOINT_ASSERT(write_length == 0);
  // Need to study this assert() WATCHPOINT_ASSERT(return_length ==
  // ptr->write_buffer_offset);
  ptr->write_buffer_offset= 0;

  return return_length;
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
    ssize_t nread= memcached_io_read(ptr, data + offset, size - offset);
    if (nread <= 0)
    {
      memcached_io_reset(ptr);
      return MEMCACHED_UNKNOWN_READ_FAILURE;
    }
    offset+= nread;
  }

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_io_readline(memcached_server_st *ptr,
                                       char *buffer_ptr,
                                       size_t size)
{
  bool line_complete= false;
  int total_nr= 0;

  while (!line_complete)
  {
    if (ptr->read_buffer_length == 0)
    {
      /*
       * We don't have any data in the buffer, so let's fill the read
       * buffer. Call the standard read function to avoid duplicating
       * the logic.
       */
      if (memcached_io_read(ptr, buffer_ptr, 1) != 1)
        return MEMCACHED_UNKNOWN_READ_FAILURE;

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
