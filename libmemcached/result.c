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
  WATCHPOINT_ASSERT(memc);
  WATCHPOINT_ASSERT(memc->options.is_initialized);

  /* Saving malloc calls :) */
  if (ptr)
  {
    memset(ptr, 0, sizeof(memcached_result_st));
  }
  else
  {
    ptr= libmemcached_calloc(memc, 1, sizeof(memcached_result_st));

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

void memcached_result_free(memcached_result_st *ptr)
{
  if (ptr == NULL)
    return;

  memcached_string_free(&ptr->value);

  if (memcached_is_allocated(ptr))
  {
    WATCHPOINT_ASSERT(ptr->root); // Without a root, that means that result was not properly initialized.
    libmemcached_free(ptr->root, ptr);
  }
  else
  {
    ptr->options.is_initialized= false;
  }
}
