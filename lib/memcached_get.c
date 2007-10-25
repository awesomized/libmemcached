#include "common.h"
#include "memcached_io.h"

static char *memcached_value_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                                   size_t *value_length, 
                                   uint16_t *flags,
                                   memcached_return *error,
                                   char load_key,
                                   unsigned int server_key)
{
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *string_ptr;
  char *end_ptr;

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);
  end_ptr= buffer + MEMCACHED_DEFAULT_COMMAND_SIZE;

  *value_length= 0;
  *flags= 0;

  *error= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

  if (*error == MEMCACHED_SUCCESS)
  {
    char *next_ptr;

    string_ptr= buffer;
    string_ptr+= 6; /* "VALUE " */

    /* We load the key */
    if (load_key)
    {
      memset(key, 0, MEMCACHED_MAX_KEY);
      *key_length= 0;

      for (; end_ptr > string_ptr && *string_ptr != ' '; string_ptr++)
      {
        *key= *string_ptr;
        key++;
        (*key_length)++;
      }
    }
    else /* Skip characters */
      for (; end_ptr > string_ptr && *string_ptr != ' '; string_ptr++);

    if (end_ptr == string_ptr)
        goto read_error;

    /* Flags fetch move past space */
    string_ptr++;
    if (end_ptr == string_ptr)
        goto read_error;
    for (next_ptr= string_ptr; end_ptr > string_ptr && *string_ptr != ' '; string_ptr++);
    *flags= (uint16_t)strtol(next_ptr, &string_ptr, 10);

    if (end_ptr == string_ptr)
        goto read_error;

    /* Length fetch move past space*/
    string_ptr++;
    if (end_ptr == string_ptr)
        goto read_error;

    for (next_ptr= string_ptr; end_ptr > string_ptr && *string_ptr != ' '; string_ptr++);
    *value_length= (size_t)strtoll(next_ptr, &string_ptr, 10);

    if (end_ptr == string_ptr)
        goto read_error;

    /* Skip past the \r\n */
    string_ptr+= 2;

    if (end_ptr < string_ptr)
        goto read_error;

    if (*value_length)
    {
      size_t read_length;
      size_t to_read;
      char *value;
      char *value_ptr;

      /* We add two bytes so that we can walk the \r\n */
      value= (char *)malloc(((*value_length) +2) * sizeof(char));
      if (!value)
      {
        *value_length= 0;
        *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
        return NULL;
      }
      memset(value, 0, ((*value_length) +2) * sizeof(char));

      value_ptr= value;
      read_length= 0;
      /* 
        We read the \r\n into the string since not doing so is more 
        cycles then the waster of memory to do so.

        We are null terminating through, which will most likely make
        some people lazy about using the return length.
      */
      to_read= (*value_length) + 2;

      read_length= memcached_io_read(ptr, server_key,
                                     value_ptr, to_read);

      if (read_length != (size_t)(*value_length + 2))
      {
        free(value);
        goto read_error;
      }

      value[*value_length]= 0;
      value[(*value_length) + 1]= 0;

      return value;
    }
  }
  else if (*error == MEMCACHED_END)
    *error= MEMCACHED_NOTFOUND;

  return NULL;
read_error:
  *error= MEMCACHED_PARTIAL_READ;
  return NULL;
}

/* 
  What happens if no servers exist?
*/
char *memcached_get(memcached_st *ptr, char *key, size_t key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *buf_ptr= buffer;
  unsigned int server_key;
  char *value= NULL;
  LIBMEMCACHED_MEMCACHED_GET_START();

  server_key= memcached_generate_hash(ptr, key, key_length);

  *value_length= 0;
  *error= memcached_connect(ptr, server_key);

  if (*error != MEMCACHED_SUCCESS)
    goto error;

  memcpy(buf_ptr, "get ", 4);
  buf_ptr+= 4;
  memcpy(buf_ptr, key, key_length);
  buf_ptr+= key_length;
  memcpy(buf_ptr, "\r\n", 2);
  buf_ptr+= 2;

  if ((memcached_io_write(ptr, server_key, buffer, (size_t)(buf_ptr - buffer), 1)) == -1)
  {
    *error= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  value= memcached_value_fetch(ptr, key, &key_length, value_length, flags,
                               error, 0, server_key);
  if (*error == MEMCACHED_END && *value_length == 0)
  {
    *error= MEMCACHED_NOTFOUND;
    goto error;
  }
  else if (*error == MEMCACHED_END)
    assert(0); /* If this happens we have somehow messed up the fetch */
  else if (*error == MEMCACHED_SUCCESS)
  {
    memcached_return rc;
    /* We need to read END */
    rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

    if (rc != MEMCACHED_END)
    {
      *error= MEMCACHED_PROTOCOL_ERROR;
      goto error;
    }
  }
  else 
      goto error;

  LIBMEMCACHED_MEMCACHED_GET_END();

  return value;

error:
  free(value);
  *value_length= 0;

  LIBMEMCACHED_MEMCACHED_GET_END();

    return NULL;
}

memcached_return memcached_mget(memcached_st *ptr, 
                                char **keys, size_t *key_length, 
                                unsigned int number_of_keys)
{
  unsigned int x;
  memcached_return rc= MEMCACHED_NOTFOUND;
  char *cursor_key_exec;
  LIBMEMCACHED_MEMCACHED_MGET_START();
  ptr->cursor_server= 0;

  if (number_of_keys == 0)
    return MEMCACHED_NOTFOUND;

  cursor_key_exec= (char *)malloc(sizeof(char) * ptr->number_of_hosts);
  memset(cursor_key_exec, 0, sizeof(char) * ptr->number_of_hosts);

  for (x= 0; x < number_of_keys; x++)
  {
    unsigned int server_key;

    server_key= memcached_generate_hash(ptr, keys[x], key_length[x]);

    if (cursor_key_exec[server_key] == 0)
    {
      rc= memcached_connect(ptr, server_key);

      if ((memcached_io_write(ptr, server_key, "get ", 4, 0)) == -1)
      {
        memcached_quit(ptr);
        rc= MEMCACHED_SOME_ERRORS;
        break;
      }
    }

    if ((memcached_io_write(ptr, server_key, keys[x], key_length[x], 0)) == -1)
    {
      memcached_quit(ptr);
      rc= MEMCACHED_SOME_ERRORS;
      break;
    }

    if ((memcached_io_write(ptr, server_key, " ", 1, 0)) == -1)
    {
      memcached_quit(ptr);
      rc= MEMCACHED_SOME_ERRORS;
      break;
    }
    cursor_key_exec[server_key]= 1;
  }


  /*
    Should we muddle on if some servers are dead?
  */
  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (cursor_key_exec[x])
    {
      /* We need to doo something about non-connnected hosts in the future */
      if ((memcached_io_write(ptr, x, "\r\n", 2, 1)) == -1)
      {
        memcached_quit(ptr);
        rc= MEMCACHED_SOME_ERRORS;
        break;
      }

      ptr->hosts[x].cursor_active= 1;
    }
    else
      ptr->hosts[x].cursor_active= 0;
  }

  free(cursor_key_exec);

  LIBMEMCACHED_MEMCACHED_MGET_END();
  return rc;
}

char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  char *value_check;

  while (ptr->cursor_server < ptr->number_of_hosts)
  {
    if (!ptr->hosts[ptr->cursor_server].cursor_active)
    {
      ptr->cursor_server++;
      continue;
    }

    value_check= memcached_value_fetch(ptr, key, key_length, value_length, flags,
                                       error, 1, ptr->cursor_server);
    
    if (*error == MEMCACHED_NOTFOUND)
      ptr->cursor_server++;
    else if (*error == MEMCACHED_END && *value_length == 0)
      return NULL;
    else if (*error == MEMCACHED_END)
      assert(0); /* If this happens we have somehow messed up the fetch */
    else if (*error != MEMCACHED_SUCCESS)
      return NULL;
    else
      return value_check;

  }

  *value_length= 0;
  return NULL;
}
