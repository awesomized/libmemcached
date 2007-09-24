#include <memcached.h>

static char *memcached_value_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                                   size_t *value_length, 
                                   uint16_t *flags,
                                   memcached_return *error,
                                   char load_key,
                                   unsigned int server_key)
{
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *string_ptr;

  *value_length= 0;

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);
  *error= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

  if (*error == MEMCACHED_SUCCESS)
  {
    char *end_ptr;

    string_ptr= buffer;
    string_ptr+= 6; /* "VALUE " */

    /* We load the key */
    if (load_key)
    {
      memset(key, 0, MEMCACHED_MAX_KEY);
      for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++)
      {
        *key= *end_ptr;
        key++;
      }
    }
    else /* Skip characters */
      for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);

    /* Flags fetch */
    string_ptr= end_ptr + 1;
    for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);
    *flags= (uint16_t)strtol(string_ptr, &end_ptr, 10);

    /* Length fetch */
    string_ptr= end_ptr + 1;
    for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);
    *value_length= strtoll(string_ptr, &end_ptr, 10);

    /* Skip past the \r\n */
    string_ptr= end_ptr +2;

    if (*value_length)
    {
      size_t read_length;
      char *value;

      /* We add two bytes so that we can walk the \r\n */
      value= (char *)malloc(((*value_length) +2) * sizeof(char));

      if (!value)
      {
        *value_length= 0;
        *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
        return NULL;
      }

      read_length= read(ptr->hosts[server_key].fd, value, (*value_length)+2);

      if ((read_length -2) != *value_length)
      {
        free(value);
        *error= MEMCACHED_PARTIAL_READ;

        return NULL;
      }

      return value;
    }
  }

  return NULL;
}

char *memcached_get(memcached_st *ptr, char *key, size_t key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  size_t send_length;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  *value_length= 0;
  *error= memcached_connect(ptr);

  if (*error != MEMCACHED_SUCCESS)
    return NULL;

  server_key= memcached_generate_hash(key, key_length) % ptr->number_of_hosts;

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, "get %.*s\r\n", 
                        (int)key_length, key);
  if (*error != MEMCACHED_SUCCESS)
    return NULL;

  if ((write(ptr->hosts[server_key].fd, buffer, send_length) == -1))
  {
    *error= MEMCACHED_WRITE_FAILURE;
    return NULL;
  }

  return memcached_value_fetch(ptr, key, &key_length, value_length, flags,
                               error, 0, server_key);
}

memcached_return memcached_mget(memcached_st *ptr, 
                                char **keys, size_t *key_length, 
                                unsigned int number_of_keys)
{
  char buffer[HUGE_STRING_LEN];
  char *buffer_ptr;
  unsigned int x;
  memcached_return rc;

  ptr->cursor_server= 0;
  memset(buffer, 0, HUGE_STRING_LEN);

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  memcpy(buffer, "get", strlen("get"));
  buffer_ptr= buffer;
  buffer_ptr+=strlen("get");

  for (x= 0; x < number_of_keys; x++)
  {
    *buffer_ptr= ' ';
    buffer_ptr++;
    memcpy(buffer_ptr, keys[x], key_length[x]);
    buffer_ptr+= key_length[x];
  }

  memcpy(buffer_ptr, "\r\n", 2);
  buffer_ptr+=2;

  /*
    This must be fixed. Right now we hit every server, and send keys
    to all servers. We should fix this quickly.
  */
  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if ((write(ptr->hosts[x].fd, buffer, (size_t)(buffer_ptr - buffer)) == -1))
    {
      memcached_quit(ptr);
      rc= MEMCACHED_SOME_ERRORS;
    }
  }

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
    value_check= memcached_value_fetch(ptr, key, key_length, value_length, flags,
                                       error, 1, ptr->cursor_server);
    
    if (*error == MEMCACHED_NOTFOUND)
      ptr->cursor_server++;
    else
      return value_check;
  }

  return NULL;
}
