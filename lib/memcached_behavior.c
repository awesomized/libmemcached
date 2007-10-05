#include "common.h" 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

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
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    /* We quit all connections so we can reset the sockets */
    memcached_quit(ptr);
    ptr->flags|= MEM_TCP_NODELAY;
    break;
  case MEMCACHED_BEHAVIOR_TCP_DELAY:
    /* We quit all connections so we can reset the sockets */
    memcached_quit(ptr);
    ptr->flags+= MEM_TCP_NODELAY;
  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    ptr->flags+= MEM_USE_MD5;
    break;
  }

  return MEMCACHED_SUCCESS;
}
