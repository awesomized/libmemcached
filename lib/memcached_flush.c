#include "common.h"

memcached_return memcached_flush(memcached_st *ptr, time_t expiration)
{
  unsigned int x;
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  LIBMEMCACHED_MEMCACHED_FLUSH_START();

  if (ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (expiration)
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all %llu\r\n", (unsigned long long)expiration);
    else
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all\r\n");

    rc= memcached_do(ptr, x, buffer, send_length, 1);

    if (rc == MEMCACHED_SUCCESS)
      (void)memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, x);
  }

  LIBMEMCACHED_MEMCACHED_FLUSH_END();
  return MEMCACHED_SUCCESS;
}
