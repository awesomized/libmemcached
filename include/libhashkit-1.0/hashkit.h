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

#pragma once

#if !defined(__cplusplus)
#  include <stdbool.h>
#  include <inttypes.h>
#else
#  include <cinttypes>
#endif
#include <sys/types.h>

#include <libhashkit-1.0/visibility.h>
#include <libhashkit-1.0/configure.h>
#include <libhashkit-1.0/types.h>
#include <libhashkit-1.0/has.h>
#include <libhashkit-1.0/algorithm.h>
#include <libhashkit-1.0/behavior.h>
#include <libhashkit-1.0/digest.h>
#include <libhashkit-1.0/function.h>
#include <libhashkit-1.0/str_algorithm.h>
#include <libhashkit-1.0/strerror.h>
#include <libhashkit-1.0/string.h>

struct hashkit_st {
  struct hashkit_function_st {
    hashkit_hash_fn function;
    void *context;
  } base_hash, distribution_hash;

  struct {
    bool is_base_same_distributed : 1;
  } flags;

  struct {
    bool is_allocated : 1;
  } options;

  void *_key;
};

#ifdef __cplusplus
extern "C" {
#endif

HASHKIT_API
hashkit_st *hashkit_create(hashkit_st *hash);

HASHKIT_API
hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *ptr);

HASHKIT_API
bool hashkit_compare(const hashkit_st *first, const hashkit_st *second);

HASHKIT_API
void hashkit_free(hashkit_st *hash);

HASHKIT_API
hashkit_string_st *hashkit_encrypt(hashkit_st *, const char *source, size_t source_length);

HASHKIT_API
hashkit_string_st *hashkit_decrypt(hashkit_st *, const char *source, size_t source_length);

HASHKIT_API
bool hashkit_key(hashkit_st *kit, const char *key, const size_t key_length);

#ifdef __cplusplus
} // extern "C"
#endif
