#include <memcached.h>

memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration)
{
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  if (expiration)
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s %u\r\n", key_length, key, expiration);
  else
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s\r\n", key_length, key);
  if ((write(ptr->hosts[0].fd, buffer, send_length) == -1))
  {
    fprintf(stderr, "failed set on %.*s TCP\n", key_length+1, key);

    return MEMCACHED_WRITE_FAILURE;
  }

  return memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);
}
