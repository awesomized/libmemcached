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

#include <cassert>

struct memcached_array_st {
  Memcached *root;
  size_t size;
  char c_str[];
};

memcached_array_st *memcached_array_clone(Memcached *memc, const memcached_array_st *original) {
  if (original) {
    return memcached_strcpy(memc, original->c_str, original->size);
  }

  return NULL;
}

memcached_array_st *memcached_strcpy(Memcached *memc, const char *str, size_t str_length) {
  assert(memc);
  assert(str);
  assert(str_length);

  memcached_array_st *array = (struct memcached_array_st *) libmemcached_malloc(
      memc, sizeof(struct memcached_array_st) + str_length + 1);

  if (array) {
    array->root = memc;
    array->size = str_length; // We don't count the NULL ending
    memcpy(array->c_str, str, str_length);
    array->c_str[str_length] = 0;
  }

  return array;
}

bool memcached_array_is_null(memcached_array_st *array) {
  if (array) {
    return false;
  }

  return true;
}

memcached_string_t memcached_array_to_string(memcached_array_st *array) {
  assert(array);
  assert(array->c_str);
  assert(array->size);
  memcached_string_t tmp;
  tmp.c_str = array->c_str;
  tmp.size = array->size;

  return tmp;
}

void memcached_array_free(memcached_array_st *array) {
  if (array) {
    WATCHPOINT_ASSERT(array->root);
    libmemcached_free(array->root, array);
  }
}

size_t memcached_array_size(memcached_array_st *array) {
  if (array) {
    return array->size;
  }

  return 0;
}

const char *memcached_array_string(memcached_array_st *array) {
  if (array) {
    return array->c_str;
  }

  return NULL;
}
