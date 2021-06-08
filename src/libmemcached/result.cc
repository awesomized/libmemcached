/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "libmemcached/common.h"

static inline void _result_init(memcached_result_st *self, Memcached *memc) {
  self->item_flags = 0;
  self->item_expiration = 0;
  self->key_length = 0;
  self->item_cas = 0;
  self->root = memc;
  self->numeric_value = UINT64_MAX;
  self->count = 0;
  self->item_key[0] = 0;
}

memcached_result_st *memcached_result_create(const memcached_st *shell, memcached_result_st *ptr) {
  const Memcached *memc = memcached2Memcached(shell);

  /* Saving malloc calls :) */
  if (ptr) {
    ptr->options.is_allocated = false;
  } else {
    ptr = libmemcached_xmalloc(memc, memcached_result_st);

    if (not ptr) {
      return NULL;
    }

    ptr->options.is_allocated = true;
  }

  ptr->options.is_initialized = true;

  _result_init(ptr, (memcached_st *) memc);

  WATCHPOINT_SET(ptr->value.options.is_initialized = false);
  memcached_string_create((memcached_st *) memc, &ptr->value, 0);
  WATCHPOINT_ASSERT_INITIALIZED(&ptr->value);
  WATCHPOINT_ASSERT(ptr->value.string == NULL);

  return ptr;
}

void memcached_result_reset(memcached_result_st *ptr) {
  ptr->key_length = 0;
  memcached_string_reset(&ptr->value);
  ptr->item_flags = 0;
  ptr->item_cas = 0;
  ptr->item_expiration = 0;
  ptr->numeric_value = UINT64_MAX;
}

void memcached_result_free(memcached_result_st *ptr) {
  if (ptr == NULL) {
    return;
  }

  memcached_string_free(&ptr->value);
  ptr->numeric_value = UINT64_MAX;

  if (memcached_is_allocated(ptr)) {
    WATCHPOINT_ASSERT(
        ptr->root); // Without a root, that means that result was not properly initialized.
    libmemcached_free(ptr->root, ptr);
  } else {
    ptr->count = 0;
    ptr->options.is_initialized = false;
  }
}

void memcached_result_reset_value(memcached_result_st *ptr) {
  memcached_string_reset(&ptr->value);
}

memcached_return_t memcached_result_set_value(memcached_result_st *ptr, const char *value,
                                              size_t length) {
  if (memcached_failed(memcached_string_append(&ptr->value, value, length))) {
    return memcached_set_errno(*ptr->root, errno, MEMCACHED_AT);
  }

  return MEMCACHED_SUCCESS;
}

const char *memcached_result_key_value(const memcached_result_st *self) {
  return self->key_length ? self->item_key : NULL;
}

size_t memcached_result_key_length(const memcached_result_st *self) {
  return self->key_length;
}

const char *memcached_result_value(const memcached_result_st *self) {
  const memcached_string_st *sptr = &self->value;
  return memcached_string_value(sptr);
}

size_t memcached_result_length(const memcached_result_st *self) {
  const memcached_string_st *sptr = &self->value;
  return memcached_string_length(sptr);
}

char *memcached_result_take_value(memcached_result_st *self) {
  memcached_string_st *sptr = &self->value;
  return memcached_string_take_value(sptr);
}

uint32_t memcached_result_flags(const memcached_result_st *self) {
  return self->item_flags;
}

uint64_t memcached_result_cas(const memcached_result_st *self) {
  return self->item_cas;
}

void memcached_result_set_flags(memcached_result_st *self, uint32_t flags) {
  self->item_flags = flags;
}

void memcached_result_set_expiration(memcached_result_st *self, time_t expiration) {
  self->item_expiration = expiration;
}
