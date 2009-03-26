#include "common.h"
#include "memcached_io.h"

char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                      size_t *value_length, 
                      uint32_t *flags,
                      memcached_return *error)
{
  memcached_result_st *result_buffer= &ptr->result;

  if (ptr->flags & MEM_USE_UDP)
  {
    *error= MEMCACHED_NOT_SUPPORTED;
    return NULL;
  }

  while (ptr->cursor_server < ptr->number_of_hosts)
  {
    char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

    if (memcached_server_response_count(&ptr->hosts[ptr->cursor_server]) == 0)
    {
      ptr->cursor_server++;
      continue;
    }

  *error= memcached_response(&ptr->hosts[ptr->cursor_server], buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, result_buffer);

    if (*error == MEMCACHED_END) /* END means that we move on to the next */
    {
      memcached_server_response_reset(&ptr->hosts[ptr->cursor_server]);
      ptr->cursor_server++;
      continue;
    }
    else if (*error == MEMCACHED_SUCCESS)
    {
      *value_length= memcached_string_length(&result_buffer->value);
    
      if (key)
      {
        strncpy(key, result_buffer->key, result_buffer->key_length);
        *key_length= result_buffer->key_length;
      }

      if (result_buffer->flags)
        *flags= result_buffer->flags;
      else
        *flags= 0;

      return  memcached_string_c_copy(&result_buffer->value);
    }
    else
    {
      *value_length= 0;
      return NULL;
    }
  }

  ptr->cursor_server= 0;
  *value_length= 0;
  return NULL;
}

memcached_result_st *memcached_fetch_result(memcached_st *ptr,
                                            memcached_result_st *result,
                                            memcached_return *error)
{
  memcached_server_st *server;

  if (ptr->flags & MEM_USE_UDP)
  {
    *error= MEMCACHED_NOT_SUPPORTED;
    return NULL;
  }

  if (result == NULL)
    if ((result= memcached_result_create(ptr, NULL)) == NULL)
      return NULL;

  while ((server = memcached_io_get_readable_server(ptr)) != NULL) {
    char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
    *error= memcached_response(server, buffer, sizeof(buffer), result);

    if (*error == MEMCACHED_SUCCESS)
      return result;
    else if (*error == MEMCACHED_END)
      memcached_server_response_reset(server);
    else
      break;
  }

  /* We have completed reading data */
  if (result->is_allocated)
    memcached_result_free(result);
  else
    memcached_string_reset(&result->value);

  return NULL;
}

