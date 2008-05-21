#include "common.h"

memcached_return memcached_delete(memcached_st *ptr, const char *key, size_t key_length,
                                  time_t expiration)
{
  return memcached_delete_by_key(ptr, key, key_length,
                                 key, key_length, expiration);
}

memcached_return memcached_delete_by_key(memcached_st *ptr, 
                                         const char *master_key, size_t master_key_length,
                                         const char *key, size_t key_length,
                                         time_t expiration)
{
  char to_write;
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  LIBMEMCACHED_MEMCACHED_DELETE_START();

  unlikely (key_length == 0)
    return MEMCACHED_NO_KEY_PROVIDED;

  unlikely (ptr->hosts == NULL || ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  server_key= memcached_generate_hash(ptr, master_key, master_key_length);

  if (expiration)
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %s%.*s %llu\r\n", 
                          ptr->prefix_key,
                          (int)key_length, key, 
                          (unsigned long long)expiration);
  else
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %s%.*s\r\n", 
                          ptr->prefix_key,
                          (int)key_length, key);

  if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  to_write= (ptr->flags & MEM_BUFFER_REQUESTS) ? 0 : 1;

  rc= memcached_do(&ptr->hosts[server_key], buffer, send_length, to_write);
  if (rc != MEMCACHED_SUCCESS)
    goto error;

  if ((ptr->flags & MEM_BUFFER_REQUESTS))
  {
    rc= MEMCACHED_BUFFERED;
  }
  else
  {
    rc= memcached_response(&ptr->hosts[server_key], buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL);
    if (rc == MEMCACHED_DELETED)
      rc= MEMCACHED_SUCCESS;
  }

  if (rc == MEMCACHED_SUCCESS && ptr->delete_trigger)
    ptr->delete_trigger(ptr, key, key_length);

error:
  LIBMEMCACHED_MEMCACHED_DELETE_END();
  return rc;
}
