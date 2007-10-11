#include "common.h" 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

/* 
  This function is used to modify the behabior of running client.

  We quit all connections so we can reset the sockets.
*/

void set_behavior_flag(memcached_st *ptr, memcached_flags temp_flag, void *data)
{
  unsigned int *truefalse= (unsigned int *)data;

  memcached_quit(ptr);
  if (truefalse)
    ptr->flags|= temp_flag;
  else
    ptr->flags+= temp_flag;
}

memcached_return memcached_behavior_set(memcached_st *ptr, 
                                        memcached_behavior flag, 
                                        void *data)
{
  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    set_behavior_flag(ptr, MEM_NO_BLOCK, data);
    break;
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    set_behavior_flag(ptr, MEM_TCP_NODELAY, data);
    break;
  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    set_behavior_flag(ptr, MEM_USE_MD5, data);
    break;
  case MEMCACHED_BEHAVIOR_KETAMA:
    set_behavior_flag(ptr, MEM_USE_KETAMA, data);
    break;
  }

  return MEMCACHED_SUCCESS;
}

unsigned long long memcached_behavior_get(memcached_st *ptr, 
                                          memcached_behavior flag)
{
  memcached_flags temp_flag;

  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    temp_flag= MEM_NO_BLOCK;
    break;
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    temp_flag= MEM_TCP_NODELAY;
    break;
  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    temp_flag= MEM_USE_MD5;
    break;
  case MEMCACHED_BEHAVIOR_KETAMA:
    temp_flag= MEM_USE_KETAMA;
    break;
  }

  if (ptr->flags & temp_flag)
    return 1;
  else
    return 0;

  return MEMCACHED_SUCCESS;
}
