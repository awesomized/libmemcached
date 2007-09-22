#include <memcached.h>

memcached_return memcached_flush(memcached_st *ptr, time_t expiration)
{
  unsigned int x;
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    rc= MEMCACHED_SOME_ERRORS;

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (expiration)
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all %llu\r\n", (unsigned long long)expiration);
    else
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all\r\n");
    if ((write(ptr->hosts[x].fd, buffer, send_length) == -1))
      return MEMCACHED_WRITE_FAILURE;

    rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, x);

    if (rc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;
  }

  return rc;
}
