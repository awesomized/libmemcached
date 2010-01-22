#include "common.h"

memcached_return_t memcached_verbosity(memcached_st *ptr, uint32_t verbosity)
{
  uint32_t x;
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
    memcached_server_instance_st *instance=
      memcached_server_instance_fetch(ptr, x);

    rrc= memcached_do(instance, buffer, send_length, true);
    if (rrc != MEMCACHED_SUCCESS)
    {
      rc= MEMCACHED_SOME_ERRORS;
      continue;
    }

    unlikely (ptr->flags.use_udp)
      continue;

    rrc= memcached_response(instance, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL);
    if (rrc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;
  }

  return rc;
}
