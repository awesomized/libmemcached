#include <memcached.h>

static memcached_return memcached_auto(memcached_st *ptr, 
                                       char *verb,
                                       char *key, size_t key_length,
                                       unsigned int offset,
                                       unsigned int *value)
{
  size_t send_length, sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  server_key= memcached_generate_hash(key, key_length) % ptr->number_of_hosts;

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %u\r\n", verb, 
                        (int)key_length, key,
                        offset);
  if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
    return MEMCACHED_WRITE_FAILURE;
  sent_length= write(ptr->hosts[server_key].fd, buffer, send_length);
  if (sent_length == -1)
  {
    fprintf(stderr, "error %s: write: %m\n", __FUNCTION__);
    return MEMCACHED_WRITE_FAILURE;
  }
  if (sent_length != send_length)
  {
    fprintf(stderr, "error %s: short write %d %d: %m\n",
	    __FUNCTION__, sent_length, send_length);
    return MEMCACHED_WRITE_FAILURE;
  }

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);
  send_length= read(ptr->hosts[server_key].fd, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);

  if (!memcmp(buffer, "ERROR\r\n", MEMCACHED_DEFAULT_COMMAND_SIZE))
  {
    *value= 0;
    rc= MEMCACHED_PROTOCOL_ERROR;
  }
  else if (!memcmp(buffer, "NOT_FOUND\r\n", MEMCACHED_DEFAULT_COMMAND_SIZE))
  {
    *value= 0;
    rc= MEMCACHED_NOTFOUND;
  }
  else
  {
    *value= strtol(buffer, (char **)NULL, 10);
    rc= MEMCACHED_SUCCESS;
  }

  return rc;
}

memcached_return memcached_increment(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     unsigned int *value)
{
  return memcached_auto(ptr, "incr", key, key_length, offset, value);
}

memcached_return memcached_decrement(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     unsigned int *value)
{
  return memcached_auto(ptr, "decr", key, key_length, offset, value);
}
