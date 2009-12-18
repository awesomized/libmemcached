/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Functions to manipulate the result structure.
 *
 */

/*
  memcached_result_st are used to internally represent the return values from
  memcached. We use a structure so that long term as identifiers are added
  to memcached we will be able to absorb new attributes without having
  to addjust the entire API.
*/
#include "common.h"

memcached_result_st *memcached_result_create(memcached_st *memc,
                                             memcached_result_st *ptr)
{
  WATCHPOINT_ASSERT(memc && memc->options.is_initialized);

  /* Saving malloc calls :) */
  if (ptr)
  {
    memset(ptr, 0, sizeof(memcached_result_st));
  }
  else
  {
    ptr= memc->call_calloc(memc, 1, sizeof(memcached_result_st));

    if (ptr == NULL)
      return NULL;
    ptr->options.is_allocated= true;
  }

  ptr->options.is_initialized= true;

  ptr->root= memc;
  memcached_string_create(memc, &ptr->value, 0);
  WATCHPOINT_ASSERT_INITIALIZED(&ptr->value);
  WATCHPOINT_ASSERT(ptr->value.string == NULL);

  return ptr;
}

void memcached_result_reset(memcached_result_st *ptr)
{
  ptr->key_length= 0;
  memcached_string_reset(&ptr->value);
  ptr->flags= 0;
  ptr->cas= 0;
  ptr->expiration= 0;
}

/*
  NOTE turn into macro
*/
memcached_return_t memcached_result_set_value(memcached_result_st *ptr, const char *value, size_t length)
{
  return memcached_string_append(&ptr->value, value, length);
}

void memcached_result_free(memcached_result_st *ptr)
{
  if (ptr == NULL)
    return;

  memcached_string_free(&ptr->value);

  if (memcached_is_allocated(ptr))
  {
    if (ptr->root != NULL)
    {
      ptr->root->call_free(ptr->root, ptr);
    }
    else
    {
      free(ptr);
    }
  }
  else
  {
    ptr->options.is_initialized= false;
  }
}


char *memcached_result_value(memcached_result_st *ptr)
{
  memcached_string_st *sptr= &ptr->value;
  return memcached_string_value(sptr);
}

size_t memcached_result_length(memcached_result_st *ptr)
{
  memcached_string_st *sptr= &ptr->value;
  return memcached_string_length(sptr);
}

