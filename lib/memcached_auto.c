#include "common.h"

static memcached_return memcached_auto(memcached_st *ptr, 
                                       char *verb,
                                       char *key, size_t key_length,
                                       unsigned int offset,
                                       unsigned int *value)
{
  size_t send_length, sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  unsigned int server_key;

  server_key= memcached_generate_hash(ptr, key, key_length);

  if ((rc= memcached_connect(ptr, server_key)) != MEMCACHED_SUCCESS)
    return rc;

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %u\r\n", verb, 
                        (int)key_length, key,
                        offset);
  if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
    return MEMCACHED_WRITE_FAILURE;
  sent_length= memcached_io_write(ptr, server_key, buffer, send_length, 1);

  if (sent_length == -1 || sent_length != send_length)
    return MEMCACHED_WRITE_FAILURE;

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);

  rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, server_key);

  /* 
    So why recheck responce? Because the protocol is brain dead :)
    The number returned might end up equaling one of the string 
    values. Less chance of a mistake with memcmp() so we will 
    use it. We still called memcached_response() though since it
    worked its magic for non-blocking IO.
  */
  if (!memcmp(buffer, "ERROR\r\n", MEMCACHED_DEFAULT_COMMAND_SIZE))
  {
    *value= 0;
    rc= MEMCACHED_PROTOCOL_ERROR;
  }
  else if (!memcmp(buffer, "NOT_FOUND\r\n", MEMCACHED_DEFAULT_COMMAND_SIZE))
  {
    *value= 0;
    rc= MEMCACHED_NOTFOUND;
  }
  else
  {
    *value= strtol(buffer, (char **)NULL, 10);
    rc= MEMCACHED_SUCCESS;
  }

  return rc;
}

memcached_return memcached_increment(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     unsigned int *value)
{
  memcached_return rc;

  LIBMEMCACHED_MEMCACHED_INCREMENT_START();
  rc= memcached_auto(ptr, "incr", key, key_length, offset, value);
  LIBMEMCACHED_MEMCACHED_INCREMENT_END();

  return rc;
}

memcached_return memcached_decrement(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     unsigned int *value)
{
  memcached_return rc;

  LIBMEMCACHED_MEMCACHED_DECREMENT_START();
  rc= memcached_auto(ptr, "decr", key, key_length, offset, value);
  LIBMEMCACHED_MEMCACHED_DECREMENT_END();

  return rc;
}
