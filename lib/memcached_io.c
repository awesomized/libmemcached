/*
  Basic socket buffered IO
*/

#include "common.h"
#include "memcached_io.h"
#include <sys/select.h>

ssize_t memcached_io_read(memcached_st *ptr, unsigned  int server_key,
                          char *buffer, size_t length)
{
  char *buffer_ptr;

  buffer_ptr= buffer;

  while (length)
  {
    if (!ptr->read_buffer_length)
    {
      size_t data_read;

      while (1)
      {
        if (ptr->flags & MEM_NO_BLOCK)
        {
          while (1)
          {
            int select_return;
            struct timeval local_tv;
            fd_set set;

            memset(&local_tv, 0, sizeof(struct timeval));

            local_tv.tv_sec= 0;
            local_tv.tv_usec= 300;

            FD_ZERO(&set);
            FD_SET(ptr->hosts[server_key].fd, &set);

            select_return= select(1, &set, NULL, NULL, &local_tv);

            if (select_return == -1)
            {
              ptr->my_errno= errno;
              return -1;
            }
            else if (!select_return)
              break;
          }
        }

        data_read= recv(ptr->hosts[server_key].fd, 
                        ptr->read_buffer, 
                        MEMCACHED_MAX_BUFFER, 0);
        if (data_read == -1)
        {
          switch (errno)
          {
          case EAGAIN:
            break;
          default:
            {
              ptr->my_errno= errno;
              return -1;
            }
          }
        }
        else if (data_read)
          break;
        /* If zero, just keep looping */
      }

      ptr->read_buffer_length= data_read;
      ptr->read_ptr= ptr->read_buffer;
    }

    *buffer_ptr= *ptr->read_ptr;
    length--;
    ptr->read_ptr++;
    ptr->read_buffer_length--;
    buffer_ptr++;
  }

  return (size_t)(buffer_ptr - buffer);
}

ssize_t memcached_io_write(memcached_st *ptr, unsigned int server_key,
                        char *buffer, size_t length, char with_flush)
{
  unsigned long long x;

  for (x= 0; x < length; x++)
  {
    ptr->write_buffer[ptr->write_buffer_offset]= buffer[x];
    ptr->write_buffer_offset++;

    if (ptr->write_buffer_offset == MEMCACHED_MAX_BUFFER)
    {
      size_t sent_length;

      sent_length= memcached_io_flush(ptr, server_key);

      assert(sent_length == MEMCACHED_MAX_BUFFER);
      ptr->write_buffer_offset= 0;
    }
  }

  if (with_flush)
  {
    if (memcached_io_flush(ptr, server_key) == -1)
      return -1;
  }

  return length;
}

ssize_t memcached_io_flush(memcached_st *ptr, unsigned int server_key)
{
  size_t sent_length;
  char *write_ptr= ptr->write_buffer;
  size_t write_length= ptr->write_buffer_offset;
  unsigned int loop= 1;

  if (ptr->write_buffer_offset == 0)
    return 0;

  while (write_length)
  {
    if (ptr->flags & MEM_NO_BLOCK)
    {

      while (1)
      {
        struct timeval local_tv;
        fd_set set;
        int select_return;

        local_tv.tv_sec= 0;
        local_tv.tv_usec= 300 * loop;

        FD_ZERO(&set);
        FD_SET(ptr->hosts[server_key].fd, &set);

        select_return= select(1, NULL, &set, NULL, &local_tv);

        if (select_return == -1)
        {
          ptr->my_errno= errno;
          return -1;
        }
        else if (!select_return)
          break;
      }
    }

    sent_length= 0;
#ifdef orig
    if ((sent_length= send(ptr->hosts[server_key].fd, write_ptr, 
                           write_length, 0)) == -1)
#endif
      if ((sent_length= write(ptr->hosts[server_key].fd, write_ptr, 
                              write_length)) == -1)
    {
      switch (errno)
      {
      case ENOBUFS:
      case EAGAIN:
        if (loop < 10)
        {
          loop++;
          break;
        }
        /* Yes, we want to fall through */
      default:
        ptr->my_errno= errno;
        return -1;
      }
    }
    else
    {
      write_ptr+= sent_length;
      write_length-= sent_length;
    }
  }

  ptr->write_buffer_offset= 0;

  return sent_length;
}

/* 
  Eventually we will just kill off the server with the problem.
*/
void memcached_io_reset(memcached_st *ptr, unsigned int server_key)
{
  ptr->write_buffer_offset= 0;
  memcached_quit(ptr);
}
