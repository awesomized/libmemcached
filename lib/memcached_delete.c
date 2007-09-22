#include <memcached.h>

memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration)
{
  size_t send_length, sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  server_key= memcached_generate_hash(key, key_length) % ptr->number_of_hosts;

  if (expiration)
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s %llu\r\n", (int)key_length, key, 
                          (unsigned long long)expiration);
  else
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s\r\n", (int)key_length, key);

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

  return memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);
}
