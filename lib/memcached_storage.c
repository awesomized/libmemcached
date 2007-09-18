/*
  Memcached library

  memcached_set()
  memcached_replace()
  memcached_add()

*/

#include <memcached.h>

static memcached_return memcached_send(memcached_st *ptr, 
                                       char *key, size_t key_length, 
                                       char *value, size_t value_length, 
                                       time_t expiration,
                                       uint16_t  flags,
                                       char *verb)
{
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %u %u %u\r\n", verb,
                        key_length, key, flags, expiration, value_length);
  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed set on %.*s TCP\n", key_length+1, key);

    return MEMCACHED_WRITE_FAILURE;
  }

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%.*s\r\n", 
                        value_length, value);
  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed set on %.*s TCP\n", key_length+1, key);

    return MEMCACHED_WRITE_FAILURE;
  }
  
  send_length= read(ptr->fd, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);

  if (send_length && buffer[0] == 'S')  /* STORED */
    return MEMCACHED_SUCCESS;
  else if (send_length && buffer[0] == 'N')  /* NOT_STORED */
    return MEMCACHED_NOTSTORED;
  else
    return MEMCACHED_READ_FAILURE;
}

memcached_return memcached_set(memcached_st *ptr, char *key, size_t key_length, 
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags)
{
  return  memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, "set");
}

memcached_return memcached_add(memcached_st *ptr, char *key, size_t key_length,
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags)
{
  return  memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, "add");
}

memcached_return memcached_replace(memcached_st *ptr, char *key, size_t key_length,
                                   char *value, size_t value_length, 
                                   time_t expiration,
                                   uint16_t  flags)
{
  return  memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, "replace");
}
