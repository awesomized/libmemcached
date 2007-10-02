/*
  Memcached library

  memcached_set()
  memcached_replace()
  memcached_add()

*/

#include "common.h"
#include "memcached_io.h"

static memcached_return memcached_send(memcached_st *ptr, 
                                       char *key, size_t key_length, 
                                       char *value, size_t value_length, 
                                       time_t expiration,
                                       uint16_t  flags,
                                       char *verb)
{
  size_t write_length;
  ssize_t sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  assert(value);
  assert(value_length);

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);

  rc= memcached_connect(ptr);
  if (rc != MEMCACHED_SUCCESS)
    return rc;

  /* Leaveing this assert in since only a library fubar could blow this */
  assert(ptr->write_buffer_offset == 0);

  server_key= memcached_generate_hash(key, key_length) % ptr->number_of_hosts;

  write_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %x %llu %zu\r\n", verb,
                        (int)key_length, key, flags, 
                        (unsigned long long)expiration, value_length);
  if (write_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  if ((sent_length= memcached_io_write(ptr, server_key, buffer, write_length)) == -1)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  /* 
    We have to flush after sending the command. Memcached is not smart enough
    to just keep reading from the socket :(
  */
  if ((sent_length= memcached_io_flush(ptr, server_key)) == -1)
    return MEMCACHED_WRITE_FAILURE;

  if ((sent_length= memcached_io_write(ptr, server_key, value, value_length)) == -1)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  if ((sent_length= memcached_io_write(ptr, server_key, "\r\n", 2)) == -1)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    goto error;
  }

  if ((sent_length= memcached_io_flush(ptr, server_key)) == -1)
    return MEMCACHED_WRITE_FAILURE;

  rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

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
                         expiration, flags, "set");
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
                         expiration, flags, "add");
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
                         expiration, flags, "replace");
  LIBMEMCACHED_MEMCACHED_REPLACE_END();
  return rc;
}
