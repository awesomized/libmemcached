#include "common.h"
#include "memcached_io.h"

/* 
  What happens if no servers exist?
*/
char *memcached_get(memcached_st *ptr, char *key, size_t key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  char *value;

  /* Request the key */
  *error= memcached_mget(ptr, &key, &key_length, 1);

  value= memcached_fetch(ptr, NULL, NULL, 
                         value_length, flags, error);

  /* This is for historical reasons */
  if (*error == MEMCACHED_END)
    *error= MEMCACHED_NOTFOUND;

  if (value == NULL)
    return NULL;

  memcached_finish(ptr);

  return value;
}

memcached_return memcached_mget(memcached_st *ptr, 
                                char **keys, size_t *key_length, 
                                unsigned int number_of_keys)
{
  unsigned int x;
  memcached_return rc= MEMCACHED_NOTFOUND;
  char *get_command= "get ";
  uint8_t get_command_length= 4

  LIBMEMCACHED_MEMCACHED_MGET_START();
  ptr->cursor_server= 0;

  if (number_of_keys == 0)
    return MEMCACHED_NOTFOUND;

  if (ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  if (ptr->flags & MEM_SUPPORT_CAS)
  {
    get_command= "gets ";
    get_command_length= 5;
  }

  memcached_finish(ptr);

  for (x= 0; x < number_of_keys; x++)
  {
    unsigned int server_key;

    server_key= memcached_generate_hash(ptr, keys[x], key_length[x]);

    if (ptr->hosts[server_key].cursor_active == 0)
    {
      rc= memcached_connect(ptr, server_key);

      if ((memcached_io_write(ptr, server_key, get_command, get_command_length, 0)) == -1)
      {
        memcached_quit_server(ptr, server_key);
        rc= MEMCACHED_SOME_ERRORS;
        continue;
      }
      ptr->hosts[server_key].cursor_active= 1;
    }

    if ((memcached_io_write(ptr, server_key, keys[x], key_length[x], 0)) == -1)
    {
      ptr->hosts[server_key].cursor_active= 0;
      memcached_quit_server(ptr, server_key);
      rc= MEMCACHED_SOME_ERRORS;
      continue;
    }

    if ((memcached_io_write(ptr, server_key, " ", 1, 0)) == -1)
    {
      ptr->hosts[server_key].cursor_active= 0;
      memcached_quit_server(ptr, server_key);
      rc= MEMCACHED_SOME_ERRORS;
      continue;
    }
  }

  /*
    Should we muddle on if some servers are dead?
  */
  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (ptr->hosts[x].cursor_active == 1)
    {
      /* We need to doo something about non-connnected hosts in the future */
      if ((memcached_io_write(ptr, x, "\r\n", 2, 1)) == -1)
      {
        memcached_quit_server(ptr, x);
        rc= MEMCACHED_SOME_ERRORS;
      }
    }
  }

  LIBMEMCACHED_MEMCACHED_MGET_END();
  return rc;
}
