/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  LibMemcached
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <libmemcached/common.h>

#ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
#endif

void initialize_binary_request(org::libmemcached::Instance* server, protocol_binary_request_header& header)
{
  server->request_id++;
  header.request.magic= PROTOCOL_BINARY_REQ;
  header.request.opaque= htons(server->request_id);
}

enum memc_read_or_write {
  MEM_READ,
  MEM_WRITE
};

/**
 * Try to fill the input buffer for a server with as much
 * data as possible.
 *
 * @param ptr the server to pack
 */
static bool repack_input_buffer(org::libmemcached::Instance* ptr)
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
    do {
      /* Just try a single read to grab what's available */
      ssize_t nr;
      if ((nr= ::recv(ptr->fd,
                    ptr->read_ptr + ptr->read_data_length,
                    MEMCACHED_MAX_BUFFER - ptr->read_data_length,
                    MSG_DONTWAIT|MSG_NOSIGNAL)) <= 0)
      {
        if (nr == 0)
        {
          memcached_set_error(*ptr, MEMCACHED_CONNECTION_FAILURE, MEMCACHED_AT);
        }
        else
        {
          switch (get_socket_errno())
          {
          case EINTR:
            continue;

#if EWOULDBLOCK != EAGAIN
          case EWOULDBLOCK:
#endif
          case EAGAIN:
#ifdef TARGET_OS_LINUX
          case ERESTART:
#endif
            break; // No IO is fine, we can just move on

          default:
            memcached_set_errno(*ptr, get_socket_errno(), MEMCACHED_AT);
          }
        }

        break;
      }
      else // We read data, append to our read buffer
      {
        ptr->read_data_length+= size_t(nr);
        ptr->read_buffer_length+= size_t(nr);

        return true;
      }
    } while (false);
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
static bool process_input_buffer(org::libmemcached::Instance* ptr)
{
  /*
   ** We might be able to process some of the response messages if we
   ** have a callback set up
 */
  if (ptr->root->callbacks != NULL)
  {
    /*
     * We might have responses... try to read them out and fire
     * callbacks
   */
    memcached_callback_st cb= *ptr->root->callbacks;

    memcached_set_processing_input((memcached_st *)ptr->root, true);

    char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
    memcached_st *root= (memcached_st *)ptr->root;
    memcached_return_t error= memcached_response(ptr, buffer, sizeof(buffer), &root->result);

    memcached_set_processing_input(root, false);

    if (error == MEMCACHED_SUCCESS)
    {
      for (unsigned int x= 0; x < cb.number_of_callback; x++)
      {
        error= (*cb.callback[x])(ptr->root, &root->result, cb.context);
        if (error != MEMCACHED_SUCCESS)
        {
          break;
        }
      }

      /* @todo what should I do with the error message??? */
    }
    /* @todo what should I do with other error messages?? */
    return true;
  }

  return false;
}

static memcached_return_t io_wait(org::libmemcached::Instance* ptr,
                                  const memc_read_or_write read_or_write)
{
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
    if (memcached_purge(ptr) == false)
    {
      return MEMCACHED_FAILURE;
    }
  }

  struct pollfd fds;
  fds.fd= ptr->fd;
  fds.events= POLLIN;
  fds.revents= 0;

  if (read_or_write == MEM_WRITE) /* write */
  {
    fds.events= POLLOUT;
    ptr->io_wait_count.write++;
  }
  else
  {
    ptr->io_wait_count.read++;
  }

  if (ptr->root->poll_timeout == 0) // Mimic 0 causes timeout behavior (not all platforms do this)
  {
    ptr->io_wait_count.timeouts++;
    return memcached_set_error(*ptr, MEMCACHED_TIMEOUT, MEMCACHED_AT);
  }

  size_t loop_max= 5;
  while (--loop_max) // While loop is for ERESTART or EINTR
  {
    int active_fd= poll(&fds, 1, ptr->root->poll_timeout);

    if (active_fd >= 1)
    {
      assert_msg(active_fd == 1 , "poll() returned an unexpected number of active file descriptors");
      if (fds.revents & POLLIN or fds.revents & POLLOUT)
      {
        return MEMCACHED_SUCCESS;
      }

      if (fds.revents & POLLHUP)
      {
        return memcached_set_error(*ptr, MEMCACHED_CONNECTION_FAILURE, MEMCACHED_AT, 
                                   memcached_literal_param("poll() detected hang up"));
      }

      if (fds.revents & POLLERR)
      {
        int local_errno= EINVAL;
        int err;
        socklen_t len= sizeof (err);
        if (getsockopt(ptr->fd, SOL_SOCKET, SO_ERROR, (char*)&err, &len) == 0)
        {
          if (err == 0) // treat this as EINTR
          {
            continue;
          }
          local_errno= err;
        }
        memcached_quit_server(ptr, true);
        return memcached_set_errno(*ptr, local_errno, MEMCACHED_AT,
                                   memcached_literal_param("poll() returned POLLHUP"));
      }
      
      return memcached_set_error(*ptr, MEMCACHED_FAILURE, MEMCACHED_AT, memcached_literal_param("poll() returned a value that was not dealt with"));
    }

    if (active_fd == 0)
    {
      ptr->io_wait_count.timeouts++;
      return memcached_set_error(*ptr, MEMCACHED_TIMEOUT, MEMCACHED_AT);
    }

    // Only an error should result in this code being called.
    int local_errno= get_socket_errno(); // We cache in case memcached_quit_server() modifies errno
    assert_msg(active_fd == -1 , "poll() returned an unexpected value");
    switch (local_errno)
    {
#ifdef TARGET_OS_LINUX
    case ERESTART:
#endif
    case EINTR:
      continue;

    case EFAULT:
    case ENOMEM:
      memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT);

    case EINVAL:
      memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT, memcached_literal_param("RLIMIT_NOFILE exceeded, or if OSX the timeout value was invalid"));

    default:
      memcached_set_errno(*ptr, local_errno, MEMCACHED_AT, memcached_literal_param("poll"));
    }

    break;
  }

  memcached_quit_server(ptr, true);

  if (memcached_has_error(ptr))
  {
    return memcached_instance_error_return(ptr);
  }

  return memcached_set_error(*ptr, MEMCACHED_CONNECTION_FAILURE, MEMCACHED_AT, 
                             memcached_literal_param("number of attempts to call io_wait() failed"));
}

static bool io_flush(org::libmemcached::Instance* ptr,
                     const bool with_flush,
                     memcached_return_t& error)
{
  /*
   ** We might want to purge the input buffer if we haven't consumed
   ** any output yet... The test for the limits is the purge is inline
   ** in the purge function to avoid duplicating the logic..
 */
  {
    WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);

    if (memcached_purge(ptr) == false)
    {
      return false;
    }
  }
  char *local_write_ptr= ptr->write_buffer;
  size_t write_length= ptr->write_buffer_offset;

  error= MEMCACHED_SUCCESS;

  WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);

  /* Looking for memory overflows */
#if defined(DEBUG)
  if (write_length == MEMCACHED_MAX_BUFFER)
    WATCHPOINT_ASSERT(ptr->write_buffer == local_write_ptr);
  WATCHPOINT_ASSERT((ptr->write_buffer + MEMCACHED_MAX_BUFFER) >= (local_write_ptr + write_length));
#endif

  while (write_length)
  {
    WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
    WATCHPOINT_ASSERT(write_length > 0);

    int flags;
    if (with_flush)
    {
      flags= MSG_NOSIGNAL|MSG_DONTWAIT;
    }
    else
    {
      flags= MSG_NOSIGNAL|MSG_DONTWAIT|MSG_MORE;
    }

    ssize_t sent_length= ::send(ptr->fd, local_write_ptr, write_length, flags);

    if (sent_length == SOCKET_ERROR)
    {
#if 0 // @todo I should look at why we hit this bit of code hard frequently
      WATCHPOINT_ERRNO(get_socket_errno());
      WATCHPOINT_NUMBER(get_socket_errno());
#endif
      switch (get_socket_errno())
      {
      case ENOBUFS:
        continue;

#if EWOULDBLOCK != EAGAIN
      case EWOULDBLOCK:
#endif
      case EAGAIN:
        {
          /*
           * We may be blocked on write because the input buffer
           * is full. Let's check if we have room in our input
           * buffer for more data and retry the write before
           * waiting..
         */
          if (repack_input_buffer(ptr) or process_input_buffer(ptr))
          {
            continue;
          }

          memcached_return_t rc= io_wait(ptr, MEM_WRITE);
          if (memcached_success(rc))
          {
            continue;
          }
          else if (rc == MEMCACHED_TIMEOUT)
          {
            return false;
          }

          memcached_quit_server(ptr, true);
          error= memcached_set_errno(*ptr, get_socket_errno(), MEMCACHED_AT);
          return false;
        }
      case ENOTCONN:
      case EPIPE:
      default:
        memcached_quit_server(ptr, true);
        error= memcached_set_errno(*ptr, get_socket_errno(), MEMCACHED_AT);
        WATCHPOINT_ASSERT(ptr->fd == INVALID_SOCKET);
        return false;
      }
    }

    ptr->io_bytes_sent+= uint32_t(sent_length);

    local_write_ptr+= sent_length;
    write_length-= uint32_t(sent_length);
  }

  WATCHPOINT_ASSERT(write_length == 0);
  ptr->write_buffer_offset= 0;

  return true;
}

memcached_return_t memcached_io_wait_for_write(org::libmemcached::Instance* ptr)
{
  return io_wait(ptr, MEM_WRITE);
}

static memcached_return_t _io_fill(org::libmemcached::Instance* ptr)
{
  ssize_t data_read;
  do
  {
    data_read= ::recv(ptr->fd, ptr->read_buffer, MEMCACHED_MAX_BUFFER, MSG_DONTWAIT|MSG_NOSIGNAL);
    if (data_read == SOCKET_ERROR)
    {
      switch (get_socket_errno())
      {
      case EINTR: // We just retry
        continue;

      case ETIMEDOUT: // OSX
#if EWOULDBLOCK != EAGAIN
      case EWOULDBLOCK:
#endif
      case EAGAIN:
#ifdef TARGET_OS_LINUX
      case ERESTART:
#endif
        {
          memcached_return_t io_wait_ret;
          if (memcached_success(io_wait_ret= io_wait(ptr, MEM_READ)))
          {
            continue;
          }

          return io_wait_ret;
        }

        /* fall through */

      case ENOTCONN: // Programmer Error
        WATCHPOINT_ASSERT(0);
      case ENOTSOCK:
        WATCHPOINT_ASSERT(0);
      case EBADF:
        assert_msg(ptr->fd != INVALID_SOCKET, "Programmer error, invalid socket");
      case EINVAL:
      case EFAULT:
      case ECONNREFUSED:
      default:
        memcached_quit_server(ptr, true);
        memcached_set_errno(*ptr, get_socket_errno(), MEMCACHED_AT);
        break;
      }

      return memcached_instance_error_return(ptr);
    }
    else if (data_read == 0)
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
      memcached_quit_server(ptr, true);
      return memcached_set_error(*ptr, MEMCACHED_CONNECTION_FAILURE, MEMCACHED_AT, 
                                 memcached_literal_param("::rec() returned zero, server has disconnected"));
    }
    ptr->io_wait_count._bytes_read+= data_read;
  } while (data_read <= 0);

  ptr->io_bytes_sent= 0;
  ptr->read_data_length= (size_t) data_read;
  ptr->read_buffer_length= (size_t) data_read;
  ptr->read_ptr= ptr->read_buffer;

  return MEMCACHED_SUCCESS;
}

memcached_return_t memcached_io_read(org::libmemcached::Instance* ptr,
                                     void *buffer, size_t length, ssize_t& nread)
{
  assert(memcached_is_udp(ptr->root) == false);
  assert_msg(ptr, "Programmer error, memcached_io_read() recieved an invalid Instance"); // Programmer error
  char *buffer_ptr= static_cast<char *>(buffer);

  if (ptr->fd == INVALID_SOCKET)
  {
#if 0
    assert_msg(int(ptr->state) <= int(MEMCACHED_SERVER_STATE_ADDRINFO), "Programmer error, invalid socket state");
#endif
    return MEMCACHED_CONNECTION_FAILURE;
  }

  while (length)
  {
    if (ptr->read_buffer_length == 0)
    {
      memcached_return_t io_fill_ret;
      if (memcached_fatal(io_fill_ret= _io_fill(ptr)))
      {
        nread= -1;
        return io_fill_ret;
      }
    }

    if (length > 1)
    {
      size_t difference= (length > ptr->read_buffer_length) ? ptr->read_buffer_length : length;

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

  nread= ssize_t(buffer_ptr - (char*)buffer);

  return MEMCACHED_SUCCESS;
}

memcached_return_t memcached_io_slurp(org::libmemcached::Instance* ptr)
{
  assert_msg(ptr, "Programmer error, invalid Instance");
  assert(memcached_is_udp(ptr->root) == false);

  if (ptr->fd == INVALID_SOCKET)
  {
    assert_msg(int(ptr->state) <= int(MEMCACHED_SERVER_STATE_ADDRINFO), "Invalid socket state");
    return MEMCACHED_CONNECTION_FAILURE;
  }

  ssize_t data_read;
  char buffer[MEMCACHED_MAX_BUFFER];
  do
  {
    data_read= ::recv(ptr->fd, ptr->read_buffer, sizeof(buffer), MSG_DONTWAIT|MSG_NOSIGNAL);
    if (data_read == SOCKET_ERROR)
    {
      switch (get_socket_errno())
      {
      case EINTR: // We just retry
        continue;

      case ETIMEDOUT: // OSX
#if EWOULDBLOCK != EAGAIN
      case EWOULDBLOCK:
#endif
      case EAGAIN:
#ifdef TARGET_OS_LINUX
      case ERESTART:
#endif
        if (memcached_success(io_wait(ptr, MEM_READ)))
        {
          continue;
        }
        return MEMCACHED_IN_PROGRESS;

        /* fall through */

      case ENOTCONN: // Programmer Error
        assert(0);
      case ENOTSOCK:
        assert(0);
      case EBADF:
        assert_msg(ptr->fd != INVALID_SOCKET, "Invalid socket state");
      case EINVAL:
      case EFAULT:
      case ECONNREFUSED:
      default:
        return MEMCACHED_CONNECTION_FAILURE; // We want this!
      }
    }
  } while (data_read > 0);

  return MEMCACHED_CONNECTION_FAILURE;
}

static bool _io_write(org::libmemcached::Instance* ptr,
                      const void *buffer, size_t length, bool with_flush,
                      size_t& written)
{
  assert(ptr->fd != INVALID_SOCKET);
  assert(memcached_is_udp(ptr->root) == false);

  const char *buffer_ptr= static_cast<const char *>(buffer);

  const size_t original_length= length;

  while (length)
  {
    char *write_ptr;
    size_t buffer_end= MEMCACHED_MAX_BUFFER;
    size_t should_write= buffer_end -ptr->write_buffer_offset;
    should_write= (should_write < length) ? should_write : length;

    write_ptr= ptr->write_buffer + ptr->write_buffer_offset;
    memcpy(write_ptr, buffer_ptr, should_write);
    ptr->write_buffer_offset+= should_write;
    buffer_ptr+= should_write;
    length-= should_write;

    if (ptr->write_buffer_offset == buffer_end)
    {
      WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);

      memcached_return_t rc;
      if (io_flush(ptr, with_flush, rc) == false)
      {
        written= original_length -length;
        return false;
      }
    }
  }

  if (with_flush)
  {
    memcached_return_t rc;
    WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
    if (io_flush(ptr, with_flush, rc) == false)
    {
      written= original_length -length;
      return false;
    }
  }

  written= original_length -length;

  return true;
}

bool memcached_io_write(org::libmemcached::Instance* ptr)
{
  size_t written;
  return _io_write(ptr, NULL, 0, true, written);
}

ssize_t memcached_io_write(org::libmemcached::Instance* ptr,
                           const void *buffer, const size_t length, const bool with_flush)
{
  size_t written;

  if (_io_write(ptr, buffer, length, with_flush, written) == false)
  {
    return -1;
  }

  return ssize_t(written);
}

bool memcached_io_writev(org::libmemcached::Instance* ptr,
                         libmemcached_io_vector_st vector[],
                         const size_t number_of, const bool with_flush)
{
  ssize_t complete_total= 0;
  ssize_t total= 0;

  for (size_t x= 0; x < number_of; x++, vector++)
  {
    complete_total+= vector->length;
    if (vector->length)
    {
      size_t written;
      if ((_io_write(ptr, vector->buffer, vector->length, false, written)) == false)
      {
        return false;
      }
      total+= written;
    }
  }

  if (with_flush)
  {
    if (memcached_io_write(ptr) == false)
    {
      return false;
    }
  }

  return (complete_total == total);
}


void memcached_io_close(org::libmemcached::Instance* ptr)
{
  if (ptr->fd == INVALID_SOCKET)
  {
    return;
  }

  /* in case of death shutdown to avoid blocking at close() */
  if (shutdown(ptr->fd, SHUT_RDWR) == SOCKET_ERROR and get_socket_errno() != ENOTCONN)
  {
    WATCHPOINT_NUMBER(ptr->fd);
    WATCHPOINT_ERRNO(get_socket_errno());
    WATCHPOINT_ASSERT(get_socket_errno());
  }

  if (closesocket(ptr->fd) == SOCKET_ERROR)
  {
    WATCHPOINT_ERRNO(get_socket_errno());
  }
  ptr->state= MEMCACHED_SERVER_STATE_NEW;
  ptr->fd= INVALID_SOCKET;
}

org::libmemcached::Instance* memcached_io_get_readable_server(memcached_st *memc)
{
#define MAX_SERVERS_TO_POLL 100
  struct pollfd fds[MAX_SERVERS_TO_POLL];
  nfds_t host_index= 0;

  for (uint32_t x= 0; x < memcached_server_count(memc) and host_index < MAX_SERVERS_TO_POLL; ++x)
  {
    org::libmemcached::Instance* instance= memcached_instance_fetch(memc, x);

    if (instance->read_buffer_length > 0) /* I have data in the buffer */
    {
      return instance;
    }

    if (instance->response_count() > 0)
    {
      fds[host_index].events= POLLIN;
      fds[host_index].revents= 0;
      fds[host_index].fd= instance->fd;
      ++host_index;
    }
  }

  if (host_index < 2)
  {
    /* We have 0 or 1 server with pending events.. */
    for (uint32_t x= 0; x< memcached_server_count(memc); ++x)
    {
      org::libmemcached::Instance* instance= memcached_instance_fetch(memc, x);

      if (instance->response_count() > 0)
      {
        return instance;
      }
    }

    return NULL;
  }

  int error= poll(fds, host_index, memc->poll_timeout);
  switch (error)
  {
  case -1:
    memcached_set_errno(*memc, get_socket_errno(), MEMCACHED_AT);
    /* FALLTHROUGH */
  case 0:
    break;

  default:
    for (nfds_t x= 0; x < host_index; ++x)
    {
      if (fds[x].revents & POLLIN)
      {
        for (uint32_t y= 0; y < memcached_server_count(memc); ++y)
        {
          org::libmemcached::Instance* instance= memcached_instance_fetch(memc, y);

          if (instance->fd == fds[x].fd)
          {
            return instance;
          }
        }
      }
    }
  }

  return NULL;
}

/*
  Eventually we will just kill off the server with the problem.
*/
void memcached_io_reset(org::libmemcached::Instance* ptr)
{
  memcached_quit_server(ptr, true);
}

/**
 * Read a given number of bytes from the server and place it into a specific
 * buffer. Reset the IO channel on this server if an error occurs.
 */
memcached_return_t memcached_safe_read(org::libmemcached::Instance* ptr,
                                       void *dta,
                                       const size_t size)
{
  size_t offset= 0;
  char *data= static_cast<char *>(dta);

  while (offset < size)
  {
    ssize_t nread;
    memcached_return_t rc;

    while (memcached_continue(rc= memcached_io_read(ptr, data + offset, size - offset, nread))) { };

    if (memcached_failed(rc))
    {
      return rc;
    }

    offset+= size_t(nread);
  }

  return MEMCACHED_SUCCESS;
}

memcached_return_t memcached_io_readline(org::libmemcached::Instance* ptr,
                                         char *buffer_ptr,
                                         size_t size,
                                         size_t& total_nr)
{
  total_nr= 0;
  bool line_complete= false;

  while (line_complete == false)
  {
    if (ptr->read_buffer_length == 0)
    {
      /*
       * We don't have any data in the buffer, so let's fill the read
       * buffer. Call the standard read function to avoid duplicating
       * the logic.
     */
      ssize_t nread;
      memcached_return_t rc= memcached_io_read(ptr, buffer_ptr, 1, nread);
      if (memcached_failed(rc) and rc == MEMCACHED_IN_PROGRESS)
      {
        memcached_quit_server(ptr, true);
        return memcached_set_error(*ptr, rc, MEMCACHED_AT);
      }
      else if (memcached_failed(rc))
      {
        return rc;
      }

      if (*buffer_ptr == '\n')
      {
        line_complete= true;
      }

      ++buffer_ptr;
      ++total_nr;
    }

    /* Now let's look in the buffer and copy as we go! */
    while (ptr->read_buffer_length and total_nr < size and line_complete == false)
    {
      *buffer_ptr = *ptr->read_ptr;
      if (*buffer_ptr == '\n')
      {
        line_complete = true;
      }
      --ptr->read_buffer_length;
      ++ptr->read_ptr;
      ++total_nr;
      ++buffer_ptr;
    }

    if (total_nr == size)
    {
      return MEMCACHED_PROTOCOL_ERROR;
    }
  }

  return MEMCACHED_SUCCESS;
}
