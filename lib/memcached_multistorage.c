/*
  Memcached library

  memcached_set()
  memcached_replace()
  memcached_add()

*/

#include "common.h"
#include "memcached_io.h"

typedef enum {
  SET_OP,
  REPLACE_OP,
  ADD_OP,
  PREPEND_OP,
  APPEND_OP,
  CAS_OP,
} memcached_storage_action;

/* Inline this */
static char *storage_op_string(memcached_storage_action verb)
{
  switch (verb)
  {
  case SET_OP:
    return "set";
  case REPLACE_OP:
    return "replace";
  case ADD_OP:
    return "add";
  case PREPEND_OP:
    return "prepend";
  case APPEND_OP:
    return "append";
  case CAS_OP:
    return "cas";
  };

  return SET_OP;
}

static memcached_return memcached_msend(memcached_st *ptr, 
                                        char *master_key, size_t master_key_length, 
                                        char **key, size_t *key_length, 
                                        char **value, size_t *value_length, 
                                        unsigned int number_of_keys,
                                        time_t expiration,
                                        uint32_t flags,
                                        uint64_t cas,
                                        memcached_storage_action verb)
{
  size_t write_length;
  ssize_t sent_length;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int x;
  unsigned int master_server_key= 0;
  memcached_return rc= MEMCACHED_SUCCESS;

  WATCHPOINT_ASSERT(key && key_length && value && value_length);

  if (key_length == 0)
    return MEMCACHED_NO_KEY_PROVIDED;

  if (ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  if (master_key && master_key_length)
    master_server_key= memcached_generate_hash(ptr, master_key, master_key_length);

  for (x= 0; x < number_of_keys; x++)
  {
    unsigned int server_key;
    memcached_return tried;

    if (master_server_key)
      server_key= master_server_key;
    else
      server_key= memcached_generate_hash(ptr, key[x], key_length[x]);


    if (cas)
      write_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                             "%s %.*s %u %llu %zu %llu\r\n", storage_op_string(verb),
                             (int)key_length[x], key[x], flags, 
                             (unsigned long long)expiration, value_length[x], 
                             (unsigned long long)cas);
    else
      write_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                             "%s %.*s %u %llu %zu\r\n", storage_op_string(verb),
                             (int)key_length[x], key[x], flags, 
                             (unsigned long long)expiration, value_length[x]);

    if (write_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
      goto error;

    tried=  memcached_do(ptr, server_key, buffer, write_length, 0);
    if (tried != MEMCACHED_SUCCESS)
      goto error;

    if ((sent_length= memcached_io_write(ptr, server_key, value[x], value_length[x], 0)) == -1)
      goto error;

    if ((sent_length= memcached_io_write(ptr, server_key, "\r\n", 2, 0)) == -1)
      goto error;

    continue;

error:
    memcached_quit_server(ptr, server_key, 1);
    continue;
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

  return rc;
}

memcached_return memcached_mset(memcached_st *ptr, 
                                char **key, size_t *key_length, 
                                char **value, size_t *value_length, 
                                unsigned int number_of_keys,
                                time_t expiration,
                                uint32_t flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_SET_START();
  rc= memcached_msend(ptr, NULL, 0, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, SET_OP);
  LIBMEMCACHED_MEMCACHED_SET_END();
  return rc;
}

memcached_return memcached_madd(memcached_st *ptr, 
                                char **key, size_t *key_length,
                                char **value, size_t *value_length, 
                                unsigned int number_of_keys,
                                time_t expiration,
                                uint32_t flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_ADD_START();
  rc= memcached_msend(ptr, NULL, 0, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, ADD_OP);
  LIBMEMCACHED_MEMCACHED_ADD_END();
  return rc;
}

memcached_return memcached_mreplace(memcached_st *ptr, 
                                    char **key, size_t *key_length,
                                    char **value, size_t *value_length, 
                                    unsigned int number_of_keys,
                                    time_t expiration,
                                    uint32_t flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_REPLACE_START();
  rc= memcached_msend(ptr, NULL, 0, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, REPLACE_OP);
  LIBMEMCACHED_MEMCACHED_REPLACE_END();
  return rc;
}

memcached_return memcached_mprepend(memcached_st *ptr, 
                                    char **key, size_t *key_length,
                                    char **value, size_t *value_length, 
                                    unsigned int number_of_keys,
                                    time_t expiration,
                                    uint32_t flags)
{
  memcached_return rc;
  rc= memcached_msend(ptr, NULL, 0, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, PREPEND_OP);
  return rc;
}

memcached_return memcached_mappend(memcached_st *ptr, 
                                   char **key, size_t *key_length,
                                   char **value, size_t *value_length, 
                                   unsigned int number_of_keys,
                                   time_t expiration,
                                   uint32_t flags)
{
  memcached_return rc;
  rc= memcached_msend(ptr, NULL, 0, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, APPEND_OP);
  return rc;
}

memcached_return memcached_mcas(memcached_st *ptr, 
                                char **key, size_t *key_length,
                                char **value, size_t *value_length, 
                                unsigned int number_of_keys,
                                time_t expiration,
                                uint32_t flags,
                                uint64_t cas)
{
  memcached_return rc;
  rc= memcached_msend(ptr, NULL, 0, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, cas, APPEND_OP);
  return rc;
}

memcached_return memcached_mset_by_key(memcached_st *ptr, 
                                       char *master_key, size_t master_key_length, 
                                       char **key, size_t *key_length, 
                                       char **value, size_t *value_length, 
                                       unsigned int number_of_keys,
                                       time_t expiration,
                                       uint32_t flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_SET_START();
  rc= memcached_msend(ptr, master_key, master_key_length, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, SET_OP);
  LIBMEMCACHED_MEMCACHED_SET_END();
  return rc;
}

memcached_return memcached_madd_by_key(memcached_st *ptr, 
                                       char *master_key, size_t master_key_length,
                                       char **key, size_t *key_length,
                                       char **value, size_t *value_length, 
                                       unsigned int number_of_keys,
                                       time_t expiration,
                                       uint32_t flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_ADD_START();
  rc= memcached_msend(ptr, master_key, master_key_length, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, ADD_OP);
  LIBMEMCACHED_MEMCACHED_ADD_END();
  return rc;
}

memcached_return memcached_mreplace_by_key(memcached_st *ptr, 
                                           char *master_key, size_t master_key_length,
                                           char **key, size_t *key_length,
                                           char **value, size_t *value_length, 
                                           unsigned int number_of_keys,
                                           time_t expiration,
                                           uint32_t flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_REPLACE_START();
  rc= memcached_msend(ptr, master_key, master_key_length, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, REPLACE_OP);
  LIBMEMCACHED_MEMCACHED_REPLACE_END();
  return rc;
}

memcached_return memcached_mprepend_by_key(memcached_st *ptr, 
                                           char *master_key, size_t master_key_length,
                                           char **key, size_t *key_length,
                                           char **value, size_t *value_length, 
                                           unsigned int number_of_keys,
                                           time_t expiration,
                                           uint32_t flags)
{
  memcached_return rc;
  rc= memcached_msend(ptr, master_key, master_key_length, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, PREPEND_OP);
  return rc;
}

memcached_return memcached_mappend_by_key(memcached_st *ptr, 
                                          char *master_key, size_t master_key_length,
                                          char **key, size_t *key_length,
                                          char **value, size_t *value_length, 
                                          unsigned int number_of_keys,
                                          time_t expiration,
                                          uint32_t flags)
{
  memcached_return rc;
  rc= memcached_msend(ptr, master_key, master_key_length, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, 0, APPEND_OP);
  return rc;
}

memcached_return memcached_mcas_by_key(memcached_st *ptr, 
                                       char *master_key, size_t master_key_length,
                                       char **key, size_t *key_length,
                                       char **value, size_t *value_length, 
                                       unsigned int number_of_keys,
                                       time_t expiration,
                                       uint32_t flags,
                                       uint64_t cas)
{
  memcached_return rc;
  rc= memcached_msend(ptr, master_key, master_key_length, 
                      key, key_length, value, value_length,
                      number_of_keys,
                      expiration, flags, cas, APPEND_OP);
  return rc;
}
