#include "common.h"
#include "memcached_io.h"

memcached_return memcached_flush_buffers(memcached_st *mem)
{
  memcached_return ret= MEMCACHED_SUCCESS;

  for (int x= 0; x < mem->number_of_hosts; ++x)
    if (memcached_io_write(&mem->hosts[x], NULL, 0, 1) == -1)
      ret= MEMCACHED_SOME_ERRORS;

  return ret;
}
