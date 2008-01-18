#include "common.h" 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

/* 
  These functions provide data and function callback support
*/

memcached_return memcached_callback_set(memcached_st *ptr, 
                                        memcached_callback flag, 
                                        void *data)
{
  switch (flag)
  {
  case MEMCACHED_CALLBACK_USER_DATA:
    {
      ptr->user_data= data;
      break;
    }
  case MEMCACHED_CALLBACK_CLEANUP_FUNCTION:
    {
      cleanup_func func= (cleanup_func)data;
      ptr->on_cleanup= func;
      break;
    }
  case MEMCACHED_CALLBACK_CLONE_FUNCTION:
    {
      clone_func func= (clone_func)data;
      ptr->on_clone= func;
      break;
    }
  default:
    return MEMCACHED_FAILURE;
  }

  return MEMCACHED_SUCCESS;
}

void *memcached_callback_get(memcached_st *ptr, 
                             memcached_callback flag,
                             memcached_return *error)
{
  switch (flag)
  {
  case MEMCACHED_CALLBACK_USER_DATA:
    {
      *error= ptr->user_data ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return (void *)ptr->user_data;
    }
  case MEMCACHED_CALLBACK_CLEANUP_FUNCTION:
    {
      *error= ptr->on_cleanup ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return (void *)ptr->on_cleanup;
    }
  case MEMCACHED_CALLBACK_CLONE_FUNCTION:
    {
      *error= ptr->on_clone ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return (void *)ptr->on_clone;
    }
  default:
      WATCHPOINT_ASSERT(0);
      *error= MEMCACHED_FAILURE;
      return NULL;
  }
}
