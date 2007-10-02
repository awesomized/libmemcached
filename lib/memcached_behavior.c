#include <memcached.h>

memcached_return memcached_behavior_set(memcached_st *ptr, memcached_behavior flag)
{
  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    ptr->flags|= MEM_NO_BLOCK;
    break;
  }

  return MEMCACHED_SUCCESS;
}
