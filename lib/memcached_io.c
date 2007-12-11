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

static ssize_t io_flush(memcached_st *ptr, unsigned int server_key, 
                                  memcached_return *error);

static memcached_return io_wait(memcached_st *ptr, unsigned int server_key, 
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
  fds[0].fd= ptr->hosts[server_key].fd;
  fds[0].events= flags;

  error= poll(fds, 1, ptr->poll_timeout);

  if (error == 1)
    return MEMCACHED_SUCCESS;
  else if (error == 0)
  {
    return MEMCACHED_TIMEOUT;
  }

  WATCHPOINT;
  /* Imposssible for anything other then -1 */
  WATCHPOINT_ASSERT(error == -1);
  memcached_quit_server(ptr, server_key, 1);

  return MEMCACHED_FAILURE;

}

void memcached_io_preread(memcached_st *ptr)
{
  unsigned int x;

  return;

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (ptr->hosts[x].cursor_active &&
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

ssize_t memcached_io_read(memcached_st *ptr, unsigned  int server_key,
                          char *buffer, size_t length)
{
  char *buffer_ptr;

  buffer_ptr= buffer;

  while (length)
  {
    if (!ptr->hosts[server_key].read_buffer_length)
    {
      size_t data_read;

      while (1)
      {
        data_read= read(ptr->hosts[server_key].fd, 
                        ptr->hosts[server_key].read_buffer, 
                        MEMCACHED_MAX_BUFFER);
        if (data_read == -1)
        {
          switch (errno)
          {
          case EAGAIN:
            {
              memcached_return rc;

              rc= io_wait(ptr, server_key, MEM_READ);

              if (rc == MEMCACHED_SUCCESS)
                continue;

              memcached_quit_server(ptr, server_key, 1);
              return -1;
            }
          default:
            {
              memcached_quit_server(ptr, server_key, 1);
              ptr->cached_errno= errno;
              return -1;
            }
          }
        }
        else if (data_read)
          break;
        /* If zero, just keep looping */
      }

      ptr->hosts[server_key].read_data_length= data_read;
      ptr->hosts[server_key].read_buffer_length= data_read;
      ptr->hosts[server_key].read_ptr= ptr->hosts[server_key].read_buffer;
    }

    if (length > 1)
    {
      size_t difference;

      difference= (length > ptr->hosts[server_key].read_buffer_length) ? ptr->hosts[server_key].read_buffer_length : length;

      memcpy(buffer_ptr, ptr->hosts[server_key].read_ptr, difference);
      length -= difference;
      ptr->hosts[server_key].read_ptr+= difference;
      ptr->hosts[server_key].read_buffer_length-= difference;
      buffer_ptr+= difference;
    }
    else
    {
      *buffer_ptr= *ptr->hosts[server_key].read_ptr;
      length--;
      ptr->hosts[server_key].read_ptr++;
      ptr->hosts[server_key].read_buffer_length--;
      buffer_ptr++;
    }
  }

  return (size_t)(buffer_ptr - buffer);
}

ssize_t memcached_io_write(memcached_st *ptr, unsigned int server_key,
                        char *buffer, size_t length, char with_flush)
{
  unsigned long long x;

  for (x= 0; x < length; x++)
  {
    if (ptr->hosts[server_key].write_ptr == 0)
      ptr->hosts[server_key].write_ptr= ptr->hosts[server_key].write_buffer;
    WATCHPOINT_ASSERT(ptr->hosts[server_key].write_ptr);
    *ptr->hosts[server_key].write_ptr= buffer[x];
    ptr->hosts[server_key].write_ptr++;
    ptr->hosts[server_key].write_buffer_offset++;

    if (ptr->hosts[server_key].write_buffer_offset == MEMCACHED_MAX_BUFFER)
    {
      memcached_return rc;
      size_t sent_length;

      sent_length= io_flush(ptr, server_key, &rc);
      if (sent_length == -1)
        return -1;

      WATCHPOINT_ASSERT(sent_length == MEMCACHED_MAX_BUFFER);
      ptr->hosts[server_key].write_ptr= ptr->hosts[server_key].write_buffer;
      ptr->hosts[server_key].write_buffer_offset= 0;
    }
  }

  if (with_flush)
  {
    memcached_return rc;
    if (io_flush(ptr, server_key, &rc) == -1)
      return -1;
  }

  return length;
}

memcached_return memcached_io_close(memcached_st *ptr, unsigned int server_key)
{
  close(ptr->hosts[server_key].fd);

  return MEMCACHED_SUCCESS;
}

static ssize_t io_flush(memcached_st *ptr, unsigned int server_key, 
                                  memcached_return *error)
{
  size_t sent_length;
  size_t return_length;
  char *write_ptr= ptr->hosts[server_key].write_buffer;
  size_t write_length= ptr->hosts[server_key].write_buffer_offset;

  *error= MEMCACHED_SUCCESS;

  if (ptr->hosts[server_key].write_buffer_offset == 0)
    return 0;

  return_length= 0;
  while (write_length)
  {
    sent_length= 0;
    if (ptr->hosts[server_key].type == MEMCACHED_CONNECTION_UDP)
    {
      sent_length= sendto(ptr->hosts[server_key].fd, 
                          write_ptr, write_length, 0, 
                          (struct sockaddr *)&ptr->hosts[server_key].address_info->ai_addr, 
                          sizeof(struct sockaddr));
    }
    else
    {
      if ((ssize_t)(sent_length= write(ptr->hosts[server_key].fd, write_ptr, 
                                       write_length)) == -1)
      {
        switch (errno)
        {
        case ENOBUFS:
          continue;
        case EAGAIN:
          {
            memcached_return rc;
            rc= io_wait(ptr, server_key, MEM_WRITE);

            if (rc == MEMCACHED_SUCCESS)
              continue;

            memcached_quit_server(ptr, server_key, 1);
            return -1;
          }
        default:
          memcached_quit_server(ptr, server_key, 1);
          ptr->cached_errno= errno;
          *error= MEMCACHED_ERRNO;
          return -1;
        }
      }
    }

    write_ptr+= sent_length;
    write_length-= sent_length;
    return_length+= sent_length;
  }

  WATCHPOINT_ASSERT(write_length == 0);
  WATCHPOINT_ASSERT(return_length == ptr->hosts[server_key].write_buffer_offset);
  ptr->hosts[server_key].write_ptr= ptr->hosts[server_key].write_buffer;
  ptr->hosts[server_key].write_buffer_offset= 0;

  return return_length;
}

/* 
  Eventually we will just kill off the server with the problem.
*/
void memcached_io_reset(memcached_st *ptr, unsigned int server_key)
{
  ptr->hosts[server_key].write_ptr= ptr->hosts[server_key].write_buffer;
  ptr->hosts[server_key].write_buffer_offset= 0;
  memcached_quit(ptr);
}
