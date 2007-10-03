#include <memcached.h>

memcached_return memcached_behavior_set(memcached_st *ptr, 
                                        memcached_behavior flag, 
                                        void *data)
{
  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    /* We quit all connections so we can reset the sockets */
    memcached_quit(ptr);
    ptr->flags|= MEM_NO_BLOCK;
    break;
  case MEMCACHED_BEHAVIOR_BLOCK:
    /* We quit all connections so we can reset the sockets */
    memcached_quit(ptr);
    ptr->flags+= MEM_NO_BLOCK;
    break;
  }

  return MEMCACHED_SUCCESS;
}
