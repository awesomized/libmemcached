#include <memcached.h>

char *memcached_get(memcached_st *ptr, char *key, size_t key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  size_t send_length;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *string_ptr;
  unsigned int server_key;

  *value_length= 0;
  *error= memcached_connect(ptr);

  if (*error != MEMCACHED_SUCCESS)
    return NULL;

  server_key= memcached_generate_hash(key, key_length) % ptr->number_of_hosts;

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, "get %.*s\r\n", 
                        key_length, key);
  if (*error != MEMCACHED_SUCCESS)
    return NULL;

  if ((send(ptr->hosts[server_key].fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed fetch on %.*s TCP\n", key_length+1, key);
    *error= MEMCACHED_WRITE_FAILURE;
    return NULL;
  }

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);
  *error= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

  if (*error == MEMCACHED_SUCCESS)
  {
    char *end_ptr;

    string_ptr= buffer;
    string_ptr+= 6; /* "VALUE " */

    /* We do nothing with the key, since we only asked for one key */
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
      size_t need_to_copy;
      char *pos_ptr;
      char *value;

      value= (char *)malloc(*value_length * sizeof(char));

      if (!value)
      {
        *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
        return NULL;
      }

      need_to_copy= (*value_length < (size_t)(buffer-string_ptr)) 
        ? *value_length 
        : (size_t)(buffer-string_ptr) ;

      pos_ptr= memcpy(value, string_ptr, need_to_copy);

      if ( need_to_copy > *value_length)
      {
        size_t read_length;
        size_t need_to_read;

        need_to_read= *value_length - need_to_copy;

        read_length= read(ptr->hosts[server_key].fd, pos_ptr, need_to_read);
        if (read_length != need_to_read)
        {
          free(value);
          *error= MEMCACHED_PARTIAL_READ;

          return NULL;
        }
      }

      return value;
    }
  }

  return NULL;
}
