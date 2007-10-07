#include "common.h" 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

/* 
  This function is used to modify the behabior of running client.

  We quit all connections so we can reset the sockets.
*/

memcached_return memcached_behavior_set(memcached_st *ptr, 
                                        memcached_behavior flag, 
                                        void *data)
{
  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    {
      unsigned int *truefalse= (unsigned int *)data;
      memcached_quit(ptr);
      if (truefalse)
        ptr->flags|= MEM_NO_BLOCK;
      else
        ptr->flags+= MEM_NO_BLOCK;
      break;
    }

  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    {
      unsigned int *truefalse= (unsigned int *)data;
      memcached_quit(ptr);
      if (truefalse)
        ptr->flags|= MEM_TCP_NODELAY;
      else
        ptr->flags+= MEM_TCP_NODELAY;
      break;
    }

  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    ptr->flags+= MEM_USE_MD5;
    break;
  }

  return MEMCACHED_SUCCESS;
}

unsigned long long memcached_behavior_get(memcached_st *ptr, 
                                          memcached_behavior flag)
{
  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    if (ptr->flags & MEM_NO_BLOCK)
      return 1;
    else
      return 0;
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    if (ptr->flags & MEM_TCP_NODELAY)
      return 1;
    else
      return 0;
  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    if (ptr->flags & MEM_USE_MD5)
      return 1;
    else
      return 0;
  }

  return MEMCACHED_SUCCESS;
}
