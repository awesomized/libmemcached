#include "common.h"
#include "memcached_io.h"

static memcached_return memcached_value_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                                              memcached_string_st *value,
                                              uint16_t *flags,
                                              uint64_t *cas,
                                              unsigned int server_key)
{
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *string_ptr;
  char *end_ptr;

  end_ptr= buffer + MEMCACHED_DEFAULT_COMMAND_SIZE;

  if (flags)
    *flags= 0;

  memcached_string_reset(value);

  rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);


  if (rc == MEMCACHED_SUCCESS)
  {
    char *next_ptr;
    size_t value_length;

    string_ptr= buffer;
    string_ptr+= 6; /* "VALUE " */

    /* We load the key */
    if (key)
    {
      *key_length= 0;

      for (; isgraph(*string_ptr); string_ptr++)
      {
        *key= *string_ptr;
        key++;
        (*key_length)++;
      }
    }
    else /* Skip characters */
      for (; isgraph(*string_ptr); string_ptr++);

    if (end_ptr == string_ptr)
        goto read_error;

    /* Flags fetch move past space */
    string_ptr++;
    if (end_ptr == string_ptr)
        goto read_error;
    for (next_ptr= string_ptr; isdigit(*string_ptr); string_ptr++);
    if (flags)
      *flags= (uint16_t)strtol(next_ptr, &string_ptr, 10);

    if (end_ptr == string_ptr)
        goto read_error;

    /* Length fetch move past space*/
    string_ptr++;
    if (end_ptr == string_ptr)
        goto read_error;

    for (next_ptr= string_ptr; isdigit(*string_ptr); string_ptr++);
    value_length= (size_t)strtoll(next_ptr, &string_ptr, 10);

    if (end_ptr == string_ptr)
        goto read_error;

    /* Skip spaces */
    if (*string_ptr == '\r')
    {
      /* Skip past the \r\n */
      string_ptr+= 2;
    }
    else
    {
      string_ptr++;
      for (next_ptr= string_ptr; isdigit(*string_ptr); string_ptr++);
      if (cas)
        *cas= (size_t)strtoll(next_ptr, &string_ptr, 10);
    }

    if (end_ptr < string_ptr)
        goto read_error;

    if (value_length)
    {
      size_t read_length;
      size_t to_read;
      char *value_ptr;

      /* We add two bytes so that we can walk the \r\n */
      rc= memcached_string_check(value, value_length+2);
      if (rc != MEMCACHED_SUCCESS)
      {
        value_length= 0;
        return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
      }

      value_ptr= memcached_string_value(value);
      read_length= 0;
      /* 
        We read the \r\n into the string since not doing so is more 
        cycles then the waster of memory to do so.

        We are null terminating through, which will most likely make
        some people lazy about using the return length.
      */
      to_read= (value_length) + 2;

      read_length= memcached_io_read(ptr, server_key,
                                     value_ptr, to_read);

      if (read_length != (size_t)(value_length + 2))
      {
        goto read_error;
      }

      /* This next bit blows the API, but this is internal....*/
      {
        char *char_ptr;
        char_ptr= memcached_string_value(value);;
        char_ptr[value_length]= 0;
        char_ptr[value_length + 1]= 0;
        memcached_string_set_length(value, value_length);
      }

      return MEMCACHED_SUCCESS;
    }
  }

  return rc;

read_error:
  return MEMCACHED_PARTIAL_READ;
}

char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  memcached_string_st *result_buffer;
  result_buffer= &ptr->result_buffer;

  if (ptr->flags & MEM_NO_BLOCK)
    memcached_io_preread(ptr);

  while (ptr->cursor_server < ptr->number_of_hosts)
  {
    if (!ptr->hosts[ptr->cursor_server].cursor_active)
    {
      ptr->cursor_server++;
      continue;
    }

    *error = memcached_value_fetch(ptr, key, key_length, result_buffer, 
                                   flags, NULL, ptr->cursor_server);
    *value_length= memcached_string_length(result_buffer);
    
    if (*error == MEMCACHED_END) /* END means that we move on to the next */
    {
      ptr->hosts[ptr->cursor_server].cursor_active= 0;
      ptr->cursor_server++;
      continue;
    }
    else if (*error == MEMCACHED_SUCCESS)
      return  memcached_string_c_copy(result_buffer);
    else
      return NULL;
  }

  ptr->cursor_server= 0;
  *value_length= 0;
  return NULL;
}

memcached_result_st *memcached_fetch_result(memcached_st *ptr, 
                                            memcached_result_st *result,
                                            memcached_return *error)
{
  if (result == NULL)
    result= memcached_result_create(ptr, NULL);

  WATCHPOINT_ASSERT(result->value.is_allocated != MEMCACHED_USED);

  if (ptr->flags & MEM_NO_BLOCK)
    memcached_io_preread(ptr);

  while (ptr->cursor_server < ptr->number_of_hosts)
  {
    if (!ptr->hosts[ptr->cursor_server].cursor_active)
    {
      ptr->cursor_server++;
      continue;
    }

    result->cas= 0; /* We do this so we do not send in any junk */
    *error= memcached_value_fetch(ptr, result->key, &result->key_length, 
                                       &result->value, 
                                       &result->flags,
                                       &result->cas,
                                       ptr->cursor_server);
    
    if (*error == MEMCACHED_END) /* END means that we move on to the next */
    {
      ptr->hosts[ptr->cursor_server].cursor_active= 0;
      ptr->cursor_server++;
      continue;
    }
    else if (*error == MEMCACHED_SUCCESS)
      return result;
    else
      return NULL;
  }

  /* We have completed reading data */
  if (result->is_allocated == MEMCACHED_ALLOCATED)
    memcached_result_free(result);
  else
    memcached_string_reset(&result->value);

  ptr->cursor_server= 0;
  return NULL;
}

memcached_return memcached_finish_server(memcached_st *ptr, unsigned int server_key)
{
  memcached_return rc;
  memcached_string_st *result_buffer;

  result_buffer= &ptr->result_buffer;

  rc= MEMCACHED_SUCCESS;
  while (rc == MEMCACHED_SUCCESS)
  {
    rc= memcached_value_fetch(ptr, NULL, NULL, result_buffer, 
                               NULL, NULL, server_key);
  }
  ptr->hosts[server_key].cursor_active= 0;

  return rc;
}

void memcached_finish(memcached_st *ptr)
{
  unsigned int x;

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (ptr->hosts[x].cursor_active)
      (void)memcached_finish_server(ptr, x);
  }

  ptr->cursor_server= 0;
}
