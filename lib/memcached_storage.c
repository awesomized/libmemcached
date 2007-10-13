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
} memcached_storage_action;

/* Inline this */
#define storage_op_string(A) A == SET_OP ? "set" : ( A == REPLACE_OP ? "replace" : "add")

static memcached_return memcached_send(memcached_st *ptr, 
                                       char *key, size_t key_length, 
                                       char *value, size_t value_length, 
                                       time_t expiration,
                                       uint16_t  flags,
                                       memcached_storage_action verb)
{
  size_t write_length;
  ssize_t sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  assert(value);
  assert(value_length);

  /* Leaving this assert in since only a library fubar could blow this */
  if (ptr->write_buffer_offset != 0)
  {
    WATCHPOINT_NUMBER(ptr->write_buffer_offset);
  }
    
  assert(ptr->write_buffer_offset == 0);

  server_key= memcached_generate_hash(ptr, key, key_length);

  rc= memcached_connect(ptr, server_key);
  if (rc != MEMCACHED_SUCCESS)
    return rc;

  write_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %x %llu %zu\r\n", storage_op_string(verb),
                        (int)key_length, key, flags, 
                        (unsigned long long)expiration, value_length);
  if (write_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  /* 
    We have to flush after sending the command. Memcached is not smart enough
    to just keep reading from the socket :(
  */
  if ((sent_length= memcached_io_write(ptr, server_key, buffer, write_length, 0)) == -1)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  if ((sent_length= memcached_io_write(ptr, server_key, value, value_length, 0)) == -1)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  if ((sent_length= memcached_io_write(ptr, server_key, "\r\n", 2, 1)) == -1)
  {
    memcached_quit_server(ptr, server_key);
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  if ((ptr->flags & MEM_NO_BLOCK) && verb == SET_OP)
  {
    rc= MEMCACHED_SUCCESS;
    memcached_server_response_increment(ptr, server_key);
  }
  else
  {
    rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);
  }

  if (rc == MEMCACHED_STORED)
    return MEMCACHED_SUCCESS;
  else 
    return rc;

error:
  memcached_io_reset(ptr, server_key);

  return rc;
}

memcached_return memcached_set(memcached_st *ptr, char *key, size_t key_length, 
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_SET_START();
  rc= memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, SET_OP);
  LIBMEMCACHED_MEMCACHED_SET_END();
  return rc;
}

memcached_return memcached_add(memcached_st *ptr, char *key, size_t key_length,
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_ADD_START();
  rc= memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, ADD_OP);
  LIBMEMCACHED_MEMCACHED_ADD_END();
  return rc;
}

memcached_return memcached_replace(memcached_st *ptr, char *key, size_t key_length,
                                   char *value, size_t value_length, 
                                   time_t expiration,
                                   uint16_t  flags)
{
  memcached_return rc;
  LIBMEMCACHED_MEMCACHED_REPLACE_START();
  rc= memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, REPLACE_OP);
  LIBMEMCACHED_MEMCACHED_REPLACE_END();
  return rc;
}
