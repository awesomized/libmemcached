#include "common.h"
#include "memcached_io.h"

/* 
  What happens if no servers exist?
*/
char *memcached_get(memcached_st *ptr, char *key, size_t key_length, 
                    size_t *value_length, 
                    uint32_t *flags,
                    memcached_return *error)
{
  return memcached_get_by_key(ptr, NULL, 0, key, key_length, value_length, 
                              flags, error);
}

char *memcached_get_by_key(memcached_st *ptr, 
                           char *master_key, size_t master_key_length, 
                           char *key, size_t key_length, 
                           size_t *value_length, 
                           uint32_t *flags,
                           memcached_return *error)
{
  unsigned int server_key;
  size_t send_length;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  memcached_result_st *result_buffer= &ptr->result;
  memcached_return rc[MEMCACHED_MAX_REPLICAS];
  uint8_t replicas= 0;

  if (ptr->number_of_hosts == 0)
  {
    *error= MEMCACHED_NO_SERVERS;
    return NULL;
  }

  if ((ptr->flags & MEM_VERIFY_KEY) && (memcachd_key_test(&key, &key_length, 1) == MEMCACHED_BAD_KEY_PROVIDED))
  {
    *value_length= 0;
    *error= MEMCACHED_BAD_KEY_PROVIDED;
    return NULL;
  }

  if (master_key)
    server_key= memcached_generate_hash(ptr, master_key, master_key_length);
  else
    server_key= memcached_generate_hash(ptr, key, key_length);

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "get %.*s\r\n", (int)key_length, key);

  do
  {
    char response_buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

    if (memcached_server_response_count(&ptr->hosts[server_key]))
    {
      if (ptr->flags & MEM_NO_BLOCK)
        (void)memcached_io_write(&ptr->hosts[server_key], NULL, 0, 1);

      while(memcached_server_response_count(&ptr->hosts[server_key]))
        (void)memcached_response(&ptr->hosts[server_key], response_buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, result_buffer);
    }

    rc[replicas]= memcached_do(&ptr->hosts[server_key], buffer, send_length, 1);
    if (rc[replicas] != MEMCACHED_SUCCESS)
      goto error;

    rc[replicas]= memcached_response(&ptr->hosts[server_key], response_buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, result_buffer);

    /* On no key found, we check the replica */
    if (rc[replicas] == MEMCACHED_END) /* END means that we move on to the next */
    {
      memcached_server_response_reset(&ptr->hosts[server_key]);
    }
    else if (rc[replicas] == MEMCACHED_SUCCESS)
    {
      *value_length= memcached_string_length(&result_buffer->value);
    
      if (result_buffer->flags)
        *flags= result_buffer->flags;

      *error= MEMCACHED_SUCCESS;
      return  memcached_string_c_copy(&result_buffer->value);
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

  /* TODO: An error on replica 1 of host down, but not found on 2, will give wrong error */
  /* This is for historical reasons */
  if (rc[0] == MEMCACHED_END)
    *error= MEMCACHED_NOTFOUND;
  else
    *error= rc[0];

  *value_length= 0;

  return NULL;
}
