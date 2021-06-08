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
#include "libmemcached/virtual_bucket.h"
#include "p9y/gettimeofday.hpp"
#include "p9y/random.hpp"

uint32_t memcached_generate_hash_value(const char *key, size_t key_length,
                                       memcached_hash_t hash_algorithm) {
  return libhashkit_digest(key, key_length, (hashkit_hash_algorithm_t) hash_algorithm);
}

static inline uint32_t generate_hash(const Memcached *ptr, const char *key, size_t key_length) {
  return hashkit_digest(&ptr->hashkit, key, key_length);
}

static uint32_t dispatch_host(const Memcached *ptr, uint32_t hash) {
  switch (ptr->distribution) {
  case MEMCACHED_DISTRIBUTION_CONSISTENT:
  case MEMCACHED_DISTRIBUTION_CONSISTENT_WEIGHTED:
  case MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA:
  case MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA_SPY: {
    uint32_t num = ptr->ketama.continuum_points_counter;
    WATCHPOINT_ASSERT(ptr->ketama.continuum);

    memcached_continuum_item_st *begin, *end, *left, *right, *middle;
    begin = left = ptr->ketama.continuum;
    end = right = ptr->ketama.continuum + num;

    while (left < right) {
      middle = left + (right - left) / 2;
      if (middle->value < hash)
        left = middle + 1;
      else
        right = middle;
    }
    if (right == end)
      right = begin;
    return right->index;
  }
  case MEMCACHED_DISTRIBUTION_MODULA:
    return hash % memcached_server_count(ptr);
  case MEMCACHED_DISTRIBUTION_RANDOM:
    return (uint32_t) random() % memcached_server_count(ptr);
  case MEMCACHED_DISTRIBUTION_VIRTUAL_BUCKET: {
    return memcached_virtual_bucket_get(ptr, hash);
  }
  default:
  case MEMCACHED_DISTRIBUTION_CONSISTENT_MAX:
    WATCHPOINT_ASSERT(0); /* We have added a distribution without extending the logic */
    return hash % memcached_server_count(ptr);
  }
  /* NOTREACHED */
}

/*
  One version is public and will not modify the distribution hash, the other will.
*/
static inline uint32_t _generate_hash_wrapper(const Memcached *ptr, const char *key,
                                              size_t key_length) {
  WATCHPOINT_ASSERT(memcached_server_count(ptr));

  if (memcached_server_count(ptr) == 1)
    return 0;

  if (ptr->flags.hash_with_namespace) {
    size_t temp_length = memcached_array_size(ptr->_namespace) + key_length;
    char temp[MEMCACHED_MAX_KEY];

    if (temp_length > MEMCACHED_MAX_KEY - 1)
      return 0;

    strncpy(temp, memcached_array_string(ptr->_namespace), memcached_array_size(ptr->_namespace));
    strncpy(temp + memcached_array_size(ptr->_namespace), key, key_length);

    return generate_hash(ptr, temp, temp_length);
  } else {
    return generate_hash(ptr, key, key_length);
  }
}

static inline void _regen_for_auto_eject(Memcached *ptr) {
  if (_is_auto_eject_host(ptr) && ptr->ketama.next_distribution_rebuild) {
    struct timeval now;

    if (gettimeofday(&now, NULL) == 0 and now.tv_sec > ptr->ketama.next_distribution_rebuild) {
      run_distribution(ptr);
    }
  }
}

void memcached_autoeject(memcached_st *ptr) {
  _regen_for_auto_eject(ptr);
}

uint32_t memcached_generate_hash_with_redistribution(memcached_st *ptr, const char *key,
                                                     size_t key_length) {
  uint32_t hash = _generate_hash_wrapper(ptr, key, key_length);

  _regen_for_auto_eject(ptr);

  return dispatch_host(ptr, hash);
}

uint32_t memcached_generate_hash(const memcached_st *shell, const char *key, size_t key_length) {
  const Memcached *ptr = memcached2Memcached(shell);
  if (ptr) {
    return dispatch_host(ptr, _generate_hash_wrapper(ptr, key, key_length));
  }

  return UINT32_MAX;
}

const hashkit_st *memcached_get_hashkit(const memcached_st *shell) {
  const Memcached *ptr = memcached2Memcached(shell);
  if (ptr) {
    return &ptr->hashkit;
  }

  return NULL;
}

memcached_return_t memcached_set_hashkit(memcached_st *shell, hashkit_st *hashk) {
  Memcached *self = memcached2Memcached(shell);
  if (self) {
    hashkit_free(&self->hashkit);
    hashkit_clone(&self->hashkit, hashk);

    return MEMCACHED_SUCCESS;
  }

  return MEMCACHED_INVALID_ARGUMENTS;
}

const char *libmemcached_string_hash(memcached_hash_t type) {
  return libhashkit_string_hash((hashkit_hash_algorithm_t) type);
}
