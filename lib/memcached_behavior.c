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
  memcached_flags temp_flag;

  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    temp_flag= MEM_NO_BLOCK;
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    temp_flag= MEM_TCP_NODELAY;
  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    temp_flag= MEM_USE_MD5;
  case MEMCACHED_BEHAVIOR_KETAMA:
    temp_flag= MEM_USE_KETAMA;
    {
      unsigned int *truefalse= (unsigned int *)data;
      memcached_quit(ptr);
      if (truefalse)
        ptr->flags|= temp_flag;
      else
        ptr->flags+= temp_flag;
      break;
    }
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
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    temp_flag= MEM_TCP_NODELAY;
  case MEMCACHED_BEHAVIOR_MD5_HASHING:
    temp_flag= MEM_USE_MD5;
  case MEMCACHED_BEHAVIOR_KETAMA:
    temp_flag= MEM_USE_KETAMA;
    if (ptr->flags & temp_flag)
      return 1;
    else
      return 0;
  }

  return MEMCACHED_SUCCESS;
}
