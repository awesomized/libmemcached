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
  char to_write;
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  LIBMEMCACHED_MEMCACHED_DELETE_START();

  if (key_length == 0)
    return MEMCACHED_NO_KEY_PROVIDED;

  if (ptr->hosts == NULL || ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  server_key= memcached_generate_hash(ptr, master_key, master_key_length);

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

  if ((ptr->flags & MEM_NO_BLOCK))
    to_write= 0;
  else
    to_write= 1;

  rc= memcached_do(ptr, server_key, buffer, send_length, to_write);
  if (rc != MEMCACHED_SUCCESS)
    goto error;

  if ((ptr->flags & MEM_NO_BLOCK))
  {
    rc= MEMCACHED_SUCCESS;
  }
  else
  {
    rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL, server_key);
    if (rc == MEMCACHED_DELETED)
      rc= MEMCACHED_SUCCESS;
  }

error:
  LIBMEMCACHED_MEMCACHED_DELETE_END();
  return rc;
}

memcached_return memcached_mdelete(memcached_st *ptr, 
                                   char **key, size_t *key_length,
                                   unsigned int number_of_keys,
                                   time_t expiration)
{
  return memcached_mdelete_by_key(ptr, NULL, 0,
                                   key, key_length,
                                   number_of_keys, expiration);

}

memcached_return memcached_mdelete_by_key(memcached_st *ptr, 
                                          char *master_key, size_t master_key_length,
                                          char **key, size_t *key_length,
                                          unsigned int number_of_keys,
                                          time_t expiration)
{
  size_t send_length;
  memcached_return rc= MEMCACHED_SUCCESS;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int master_server_key= 0;
  unsigned int x;

  LIBMEMCACHED_MEMCACHED_DELETE_START();

  if (ptr->hosts == NULL || ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  if (master_key && master_key_length)
    master_server_key= memcached_generate_hash(ptr, master_key, master_key_length);

  for (x= 0; x < number_of_keys; x++)
  {
    unsigned int server_key;

    if (master_key && master_key_length)
      server_key= master_server_key;
    else
      server_key= memcached_generate_hash(ptr, key[x], key_length[x]);

    if (expiration)
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "delete %.*s %llu\r\n", (int)(key_length[x]), key[x], 
                            (unsigned long long)expiration);
    else
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "delete %.*s\r\n", (int)(key_length[x]), key[x]);

    (void)memcached_do(ptr, server_key, buffer, send_length, 0);
  }

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (memcached_server_response_count(ptr, x))
    {
      /* We need to do something about non-connnected hosts in the future */
      if ((memcached_io_write(ptr, x, NULL, 0, 1)) == -1)
      {
        rc= MEMCACHED_SOME_ERRORS;
      }
    }
  }

  LIBMEMCACHED_MEMCACHED_DELETE_END();
  return rc;
}
