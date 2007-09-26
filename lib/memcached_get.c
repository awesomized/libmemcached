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
  unsigned int x;
  memcached_return rc;
  memcached_string_st **cursor_key_exec;

  ptr->cursor_server= 0;
  memset(buffer, 0, HUGE_STRING_LEN);

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    return rc;

  cursor_key_exec= (memcached_string_st **)malloc(sizeof(memcached_string_st *) * ptr->number_of_hosts);
  memset(cursor_key_exec, 0, sizeof(memcached_string_st *) * ptr->number_of_hosts);


  for (x= 0; x < number_of_keys; x++)
  {
    unsigned int server_key;

    server_key= memcached_generate_hash(keys[x], key_length[x]) % ptr->number_of_hosts;

    if (cursor_key_exec[server_key])
    {
      memcached_string_st *string= cursor_key_exec[server_key];

      memcached_string_append_character(ptr, string, ' ');
      memcached_string_append(ptr, string, keys[x], key_length[x]);
    }
    else
    {
      memcached_string_st *string= memcached_string_init(ptr, SMALL_STRING_LEN);

      if (!string)
        assert(0);

      memcached_string_append(ptr, string, "get ", 4);
      memcached_string_append(ptr, string, keys[x], key_length[x]);

      cursor_key_exec[server_key]= string;
    }
  }


  /*
    Should we muddle on if some servers are dead?
  */
  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (cursor_key_exec[x])
    {
      memcached_string_st *string= cursor_key_exec[x];
      memcached_string_append(ptr, string, "\r\n", 2);

      if ((write(ptr->hosts[x].fd, string->string, 
                 memcached_string_length(ptr, string)) == -1))
      {
        memcached_quit(ptr);
        rc= MEMCACHED_SOME_ERRORS;
      }
      memcached_string_free(ptr, string);
    }
  }

  free(cursor_key_exec);

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
