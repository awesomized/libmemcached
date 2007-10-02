/*
  Basic socket buffered IO
*/

#include <memcached.h>

ssize_t memcached_io_read(memcached_st *ptr, unsigned  int server_key,
                          char *buffer, size_t length)
{
  size_t x;

  for (x= 0; x < length; x++)
  {
    if (!ptr->read_buffer_length)
    {
      ptr->read_buffer_length= recv(ptr->hosts[server_key].fd, 
                                    ptr->read_buffer, 
                                    MEMCACHED_MAX_BUFFER, 0);
      ptr->read_ptr= ptr->read_buffer;

      if (ptr->read_buffer_length == -1)
        return -1;
      if (ptr->read_buffer_length == 0)
        return x;
    }
    buffer[x]= *ptr->read_ptr;
    ptr->read_ptr++;
    ptr->read_buffer_length--;
  }

  return length;
}

ssize_t memcached_io_write(memcached_st *ptr, unsigned int server_key,
                        char *buffer, size_t length)
{
  unsigned long long x;

  for (x= 0; x < length; x++)
  {
    ptr->write_buffer[ptr->write_buffer_offset]= buffer[x];
    ptr->write_buffer_offset++;
    if (ptr->write_buffer_offset == MEMCACHED_MAX_BUFFER)
    {
      size_t sent_length;

      if ((sent_length= send(ptr->hosts[server_key].fd, ptr->write_buffer, 
                             MEMCACHED_MAX_BUFFER, 0)) == -1)
        return -1;

      assert(sent_length == MEMCACHED_MAX_BUFFER);
      ptr->write_between_flush+= MEMCACHED_MAX_BUFFER;

      ptr->write_buffer_offset= 0;
    }
  }

  return length;
}

ssize_t memcached_io_flush(memcached_st *ptr, unsigned int server_key)
{
  size_t sent_length;

  if (ptr->write_buffer_offset == 0)
    return 0;

  if ((sent_length= send(ptr->hosts[server_key].fd, ptr->write_buffer, 
                         ptr->write_buffer_offset, 0)) == -1)
    return -1;

  assert(sent_length == ptr->write_buffer_offset);

  sent_length+= ptr->write_between_flush;

  ptr->write_buffer_offset= 0;
  ptr->write_between_flush= 0;

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
