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
  size_t write_length;
  ssize_t sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  rc= memcached_connect(ptr);
  assert(value);
  assert(value_length);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  server_key= memcached_generate_hash(key, key_length) % ptr->number_of_hosts;

  write_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %x %llu %zu\r\n", verb,
                        (int)key_length, key, flags, 
                        (unsigned long long)expiration, value_length);
  if ((sent_length= write(ptr->hosts[server_key].fd, buffer, write_length)) == -1)
    return MEMCACHED_WRITE_FAILURE;
  assert(write_length == sent_length);

  if ((sent_length= write(ptr->hosts[server_key].fd, value, value_length)) == -1)
    return MEMCACHED_WRITE_FAILURE;
  assert(value_length == sent_length);

  if ((sent_length= write(ptr->hosts[server_key].fd, "\r\n", 2)) == -1)
    return MEMCACHED_WRITE_FAILURE;
  assert(2 == sent_length);

  sent_length= read(ptr->hosts[server_key].fd, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);

  if (sent_length && buffer[0] == 'S')  /* STORED */
    return MEMCACHED_SUCCESS;
  else if (write_length && buffer[0] == 'N')  /* NOT_STORED */
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
