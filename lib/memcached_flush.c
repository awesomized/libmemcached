#include "common.h"

memcached_return memcached_flush(memcached_st *ptr, time_t expiration)
{
  unsigned int x;
  size_t send_length, sent_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  LIBMEMCACHED_MEMCACHED_FLUSH_START();

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    rc= MEMCACHED_SOME_ERRORS;

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);
  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (expiration)
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all %llu\r\n", (unsigned long long)expiration);
    else
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all\r\n");

    if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
      return MEMCACHED_WRITE_FAILURE;

    sent_length= send(ptr->hosts[x].fd, buffer, send_length, 0);

    if (sent_length == -1 || sent_length != send_length)
      return MEMCACHED_WRITE_FAILURE;

    rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, x);

    if (rc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;
  }

  LIBMEMCACHED_MEMCACHED_FLUSH_END();
  return rc;
}
