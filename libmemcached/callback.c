/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Change any of the possible callbacks.
 *
 */

#include "libmemcached/common.h"
#include <sys/types.h>

/*
  These functions provide data and function callback support
*/

memcached_return_t memcached_callback_set(memcached_st *ptr,
                                          const memcached_callback_t flag,
                                          void *data)
{
  switch (flag)
  {
  case MEMCACHED_CALLBACK_PREFIX_KEY:
    {
      return memcached_set_prefix_key(ptr, (char*)data, data ? strlen((char*)data) : 0);
    }
  case MEMCACHED_CALLBACK_USER_DATA:
    {
      ptr->user_data= data;
      break;
    }
  case MEMCACHED_CALLBACK_CLEANUP_FUNCTION:
    {
      memcached_cleanup_fn func= *(memcached_cleanup_fn *)&data;
      ptr->on_cleanup= func;
      break;
    }
  case MEMCACHED_CALLBACK_CLONE_FUNCTION:
    {
      memcached_clone_fn func= *(memcached_clone_fn *)&data;
      ptr->on_clone= func;
      break;
    }
#ifdef MEMCACHED_ENABLE_DEPRECATED
  case MEMCACHED_CALLBACK_MALLOC_FUNCTION:
    {
      memcached_malloc_function func= *(memcached_malloc_fn *)&data;
      ptr->call_malloc= func;
      break;
    }
  case MEMCACHED_CALLBACK_REALLOC_FUNCTION:
    {
      memcached_realloc_function func= *(memcached_realloc_fn *)&data;
      ptr->call_realloc= func;
      break;
    }
  case MEMCACHED_CALLBACK_FREE_FUNCTION:
    {
      memcached_free_function func= *(memcached_free_fn *)&data;
      ptr->call_free= func;
      break;
    }
#endif
  case MEMCACHED_CALLBACK_GET_FAILURE:
    {
      memcached_trigger_key_fn func= *(memcached_trigger_key_fn *)&data;
      ptr->get_key_failure= func;
      break;
    }
  case MEMCACHED_CALLBACK_DELETE_TRIGGER:
    {
      memcached_trigger_delete_key_fn func= *(memcached_trigger_delete_key_fn *)&data;
      ptr->delete_trigger= func;
      break;
    }
  case MEMCACHED_CALLBACK_MAX:
  default:
    return MEMCACHED_FAILURE;
  }

  return MEMCACHED_SUCCESS;
}

void *memcached_callback_get(memcached_st *ptr,
                             const memcached_callback_t flag,
                             memcached_return_t *error)
{
  memcached_return_t local_error;

  if (!error)
    error = &local_error;

  switch (flag)
  {
  case MEMCACHED_CALLBACK_PREFIX_KEY:
    {
      if (ptr->prefix_key)
      {
        *error= MEMCACHED_SUCCESS;
        return (void *)memcached_array_string(ptr->prefix_key);
      }
      else
      {
        *error= MEMCACHED_FAILURE;
        return NULL;
      }
    }
  case MEMCACHED_CALLBACK_USER_DATA:
    {
      *error= ptr->user_data ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return (void *)ptr->user_data;
    }
  case MEMCACHED_CALLBACK_CLEANUP_FUNCTION:
    {
      *error= ptr->on_cleanup ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->on_cleanup;
    }
  case MEMCACHED_CALLBACK_CLONE_FUNCTION:
    {
      *error= ptr->on_clone ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->on_clone;
    }
#ifdef MEMCACHED_ENABLE_DEPRECATED
  case MEMCACHED_CALLBACK_MALLOC_FUNCTION:
    {
      *error= ptr->call_malloc ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->call_malloc;
    }
  case MEMCACHED_CALLBACK_REALLOC_FUNCTION:
    {
      *error= ptr->call_realloc ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->call_realloc;
    }
  case MEMCACHED_CALLBACK_FREE_FUNCTION:
    {
      *error= ptr->call_free ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->call_free;
    }
#endif
  case MEMCACHED_CALLBACK_GET_FAILURE:
    {
      *error= ptr->get_key_failure ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->get_key_failure;
    }
  case MEMCACHED_CALLBACK_DELETE_TRIGGER:
    {
      *error= ptr->delete_trigger ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
      return *(void **)&ptr->delete_trigger;
    }
  case MEMCACHED_CALLBACK_MAX:
  default:
    WATCHPOINT_ASSERT(0);
    *error= MEMCACHED_FAILURE;
    return NULL;
  }
}
