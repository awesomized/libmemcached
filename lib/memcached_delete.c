#include "common.h"

memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration)
{
  size_t send_length, sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  LIBMEMCACHED_MEMCACHED_DELETE_START();

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
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  sent_length= send(ptr->hosts[server_key].fd, buffer, send_length, 0);

  if (sent_length == -1 || sent_length != send_length)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

  if (rc == MEMCACHED_DELETED)
    rc= MEMCACHED_SUCCESS;

  LIBMEMCACHED_MEMCACHED_DELETE_END();

error:
  return rc;
}
