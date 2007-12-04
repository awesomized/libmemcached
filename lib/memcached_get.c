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
  return memcached_get_by_key(ptr, NULL, 0, key, key_length, value_length, 
                              flags, error);
}

char *memcached_get_by_key(memcached_st *ptr, 
                           char *master_key, size_t master_key_length, 
                           char *key, size_t key_length, 
                           size_t *value_length, 
                           uint16_t *flags,
                           memcached_return *error)
{
  char *value;

  /* Request the key */
  *error= memcached_mget_by_key(ptr, 
                                master_key, 
                                master_key_length, 
                                &key, &key_length, 1);

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
  return memcached_mget_by_key(ptr, NULL, 0, keys, key_length, number_of_keys);
}

memcached_return memcached_mget_by_key(memcached_st *ptr, 
                                       char *master_key, size_t master_key_length,
                                       char **keys, size_t *key_length, 
                                       unsigned int number_of_keys)
{
  unsigned int x;
  memcached_return rc= MEMCACHED_NOTFOUND;
  char *get_command= "get ";
  uint8_t get_command_length= 4;
  unsigned int master_server_key= 0;

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

  if (master_key && master_key_length)
    master_server_key= memcached_generate_hash(ptr, master_key, master_key_length);

  /* 
    If a server fails we warn about errors and start all over with sending keys
    to the server.
  */
  for (x= 0; x < number_of_keys; x++)
  {
    unsigned int server_key;

    if (master_server_key)
      server_key= master_server_key;
    else
      server_key= memcached_generate_hash(ptr, keys[x], key_length[x]);

    if (ptr->hosts[server_key].cursor_active == 0)
    {
      rc= memcached_connect(ptr, server_key);

      if ((memcached_io_write(ptr, server_key, get_command, get_command_length, 0)) == -1)
      {
        rc= MEMCACHED_SOME_ERRORS;
        continue;
      }
      ptr->hosts[server_key].cursor_active= 1;
    }

    if ((memcached_io_write(ptr, server_key, keys[x], key_length[x], 0)) == -1)
    {
      ptr->hosts[server_key].cursor_active= 0;
      rc= MEMCACHED_SOME_ERRORS;
      continue;
    }

    if ((memcached_io_write(ptr, server_key, " ", 1, 0)) == -1)
    {
      ptr->hosts[server_key].cursor_active= 0;
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
        rc= MEMCACHED_SOME_ERRORS;
      }
    }
  }

  LIBMEMCACHED_MEMCACHED_MGET_END();
  return rc;
}
