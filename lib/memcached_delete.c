#include "common.h"

memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration)
{
  return memcached_delete_by_key(ptr, key, key_length,
                                 key, key_length, expiration);
}

memcached_return memcached_delete_by_key(memcached_st *ptr, 
                                         char *master_key, size_t master_key_length,
                                         char *key, size_t key_length,
                                         time_t expiration)
{
  char to_write= (ptr->flags & MEM_BUFFER_REQUESTS) ? 0 : 1;
  size_t send_length;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;
  uint8_t replicas= 0;
  memcached_return rc[MEMCACHED_MAX_REPLICAS];

  unlikely (key_length == 0)
    return MEMCACHED_NO_KEY_PROVIDED;

  unlikely (ptr->hosts == NULL || ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  server_key= memcached_generate_hash(ptr, master_key, master_key_length);

  if (expiration)
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s %llu\r\n", (int)key_length, key, 
                          (unsigned long long)expiration);
  else
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s\r\n", (int)key_length, key);

  unlikely (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
    return MEMCACHED_WRITE_FAILURE;

  do
  {
    rc[replicas]= memcached_do(&ptr->hosts[server_key], buffer, send_length, to_write);
    if (rc[replicas] != MEMCACHED_SUCCESS)
      goto error;

    if ((ptr->flags & MEM_BUFFER_REQUESTS))
    {
      rc[replicas]= MEMCACHED_BUFFERED;
    }
    else
    {
      char response_buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

      rc[replicas]= memcached_response(&ptr->hosts[server_key], response_buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL);
    }

    /* On error we just jump to the next potential server */
error:
    if (ptr->number_of_replicas > 1)
    {
      if (server_key == (ptr->number_of_hosts - 1))
        server_key= 0;
      else
        server_key++;
    }
  } while ((++replicas) < ptr->number_of_replicas);

  /* As long as one object gets stored, we count this as a success */
  while (replicas--)
  {
    if (rc[replicas] == MEMCACHED_DELETED)
      return MEMCACHED_SUCCESS;
    else if (rc[replicas] == MEMCACHED_BUFFERED)
      return MEMCACHED_BUFFERED;
  }

  return rc[0];
}
