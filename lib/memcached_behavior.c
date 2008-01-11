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
  case MEMCACHED_BEHAVIOR_SUPPORT_CAS:
    set_behavior_flag(ptr, MEM_SUPPORT_CAS, data);
    break;
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    set_behavior_flag(ptr, MEM_NO_BLOCK, data);
  case MEMCACHED_BEHAVIOR_BUFFER_REQUESTS:
    set_behavior_flag(ptr, MEM_BUFFER_REQUESTS, data);
    break;
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    set_behavior_flag(ptr, MEM_TCP_NODELAY, data);
    break;
  case MEMCACHED_BEHAVIOR_DISTRIBUTION:
    ptr->distribution= *(memcached_server_distribution *)(data);
    break;
  case MEMCACHED_BEHAVIOR_HASH:
    ptr->hash= *(memcached_hash *)(data);
    break;
  case MEMCACHED_BEHAVIOR_CACHE_LOOKUPS:
    set_behavior_flag(ptr, MEM_USE_CACHE_LOOKUPS, data);
    break;
  case MEMCACHED_BEHAVIOR_KETAMA:
    set_behavior_flag(ptr, MEM_USE_KETAMA, data);
    break;
  case MEMCACHED_BEHAVIOR_USER_DATA:
    ptr->user_data= data;
    break;
  case MEMCACHED_BEHAVIOR_POLL_TIMEOUT:
    {
      int32_t timeout= (*((int32_t *)data));

      ptr->poll_timeout= timeout;
      break;
    }
  case MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE:
    {
      ptr->send_size= (*((int *)data));
      memcached_quit(ptr);
      break;
    }
  case MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE:
    {
      ptr->recv_size= (*((int *)data));
      memcached_quit(ptr);
      break;
    }


  }

  return MEMCACHED_SUCCESS;
}

unsigned long long memcached_behavior_get(memcached_st *ptr, 
                                          memcached_behavior flag)
{
  memcached_flags temp_flag= 0;

  switch (flag)
  {
  case MEMCACHED_BEHAVIOR_SUPPORT_CAS:
    temp_flag= MEM_SUPPORT_CAS;
    break;
  case MEMCACHED_BEHAVIOR_CACHE_LOOKUPS:
    temp_flag= MEM_USE_CACHE_LOOKUPS;
    break;
  case MEMCACHED_BEHAVIOR_NO_BLOCK:
    temp_flag= MEM_NO_BLOCK;
    break;
  case MEMCACHED_BEHAVIOR_BUFFER_REQUESTS:
    temp_flag= MEM_BUFFER_REQUESTS;
    break;
  case MEMCACHED_BEHAVIOR_TCP_NODELAY:
    temp_flag= MEM_TCP_NODELAY;
    break;
  case MEMCACHED_BEHAVIOR_DISTRIBUTION:
    return ptr->distribution;
  case MEMCACHED_BEHAVIOR_HASH:
    return ptr->hash;
  case MEMCACHED_BEHAVIOR_KETAMA:
    temp_flag= MEM_USE_KETAMA;
    break;
  case MEMCACHED_BEHAVIOR_USER_DATA:
    return (unsigned long long)ptr->user_data;
  case MEMCACHED_BEHAVIOR_POLL_TIMEOUT:
    {
      return (unsigned long long)ptr->poll_timeout;
    }
  case MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE:
    {
      int sock_size;
      socklen_t sock_length= sizeof(int);

      /* We just try the first host, and if it is down we return zero */
      if ((memcached_connect(ptr, 0)) != MEMCACHED_SUCCESS)
        return 0;

      if (getsockopt(ptr->hosts[0].fd, SOL_SOCKET, 
                     SO_SNDBUF, &sock_size, &sock_length))
        return 0; /* Zero means error */

      return sock_size;
    }
  case MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE:
    {
      int sock_size;
      socklen_t sock_length= sizeof(int);

      /* We just try the first host, and if it is down we return zero */
      if ((memcached_connect(ptr, 0)) != MEMCACHED_SUCCESS)
        return 0;

      if (getsockopt(ptr->hosts[0].fd, SOL_SOCKET, 
                     SO_RCVBUF, &sock_size, &sock_length))
        return 0; /* Zero means error */

      return sock_size;
    }
  }

  WATCHPOINT_ASSERT(temp_flag); /* Programming mistake if it gets this far */
  if (ptr->flags & temp_flag)
    return 1;
  else
    return 0;

  return MEMCACHED_SUCCESS;
}
