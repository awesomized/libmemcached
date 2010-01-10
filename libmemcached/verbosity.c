#include "common.h"

memcached_return_t memcached_verbosity(memcached_st *ptr, unsigned int verbosity)
{
  unsigned int x;
  size_t send_length;
  memcached_return_t rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  send_length= (size_t) snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                                 "verbosity %u\r\n", verbosity);
  unlikely (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE)
    return MEMCACHED_WRITE_FAILURE;

  rc= MEMCACHED_SUCCESS;
  for (x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_return_t rrc;

    rrc= memcached_do(&ptr->hosts[x], buffer, send_length, 1);
    if (rrc != MEMCACHED_SUCCESS)
    {
      rc= MEMCACHED_SOME_ERRORS;
      continue;
    }

    unlikely (ptr->flags.use_udp)
      continue;

    rrc= memcached_response(&ptr->hosts[x], buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL);
    if (rrc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;
  }

  return rc;
}
