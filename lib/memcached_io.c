/*
  Basic socket buffered IO
*/

#include <memcached.h>
#include "memcached_io.h"
#include <sys/select.h>

ssize_t memcached_io_read(memcached_st *ptr, unsigned  int server_key,
                          char *buffer, size_t length)
{
  size_t x;
  char *buffer_ptr;

  buffer_ptr= buffer;

  for (x= 0, buffer_ptr= buffer; 
       x < length; x++)
  {
    if (!ptr->read_buffer_length)
    {
      if (length > 1)
      {

        size_t data_read;
        data_read= recv(ptr->hosts[server_key].fd, 
                        buffer_ptr, 
                        length - x, 0);
        if (data_read == -1)
        {
          return -1;
        }
        if (data_read == 0)
          return x;

        data_read+= x;

        return data_read;
      }
      else
      {
        size_t data_read;
try_again:

        if (ptr->flags & MEM_NO_BLOCK)
        {
          struct timeval local_tv;
          fd_set set;

          memset(&local_tv, 0, sizeof(struct timeval));

          local_tv.tv_sec= 0;
          local_tv.tv_usec= 300;

          FD_ZERO(&set);
          FD_SET(ptr->hosts[server_key].fd, &set);

          select(1, &set, NULL, NULL, &local_tv);
        }

        data_read= recv(ptr->hosts[server_key].fd, 
                        ptr->read_buffer, 
                        MEMCACHED_MAX_BUFFER, 0);
        if (data_read == -1)
        {
          if (errno == EAGAIN)
            goto try_again;
          return -1;
        }
        ptr->read_buffer_length= data_read;
        ptr->read_ptr= ptr->read_buffer;
      }

      if (ptr->read_buffer_length == -1)
        return -1;
      if (ptr->read_buffer_length == 0)
        return x;
    }
    *buffer_ptr= *ptr->read_ptr;
    buffer_ptr++;
    ptr->read_ptr++;
    ptr->read_buffer_length--;
  }

  return length;
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
    memcached_io_flush(ptr, server_key);

  return length;
}

ssize_t memcached_io_flush(memcached_st *ptr, unsigned int server_key)
{
  size_t sent_length;

  if (ptr->write_buffer_offset == 0)
    return 0;

  if (ptr->flags & MEM_NO_BLOCK)
  {
    struct timeval local_tv;
    fd_set set;

    local_tv.tv_sec= 0;
    local_tv.tv_usec= 300;

    FD_ZERO(&set);
    FD_SET(ptr->hosts[server_key].fd, &set);

    select(1, NULL, &set, NULL, &local_tv);
  }
  if ((sent_length= send(ptr->hosts[server_key].fd, ptr->write_buffer, 
                         ptr->write_buffer_offset, 0)) == -1)
  {
    return -1;
  }

  assert(sent_length == ptr->write_buffer_offset);

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
