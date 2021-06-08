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

typedef enum {
  HASHKIT_SUCCESS,
  HASHKIT_FAILURE,
  HASHKIT_MEMORY_ALLOCATION_FAILURE,
  HASHKIT_INVALID_HASH,
  HASHKIT_INVALID_ARGUMENT,
  HASHKIT_MAXIMUM_RETURN /* Always add new error code before */
} hashkit_return_t;

static inline bool hashkit_success(const hashkit_return_t rc) {
  return (rc == HASHKIT_SUCCESS);
}

static inline bool hashkit_failed(const hashkit_return_t rc) {
  return (rc != HASHKIT_SUCCESS);
}

typedef enum {
  HASHKIT_HASH_DEFAULT = 0, // hashkit_one_at_a_time()
  HASHKIT_HASH_MD5,
  HASHKIT_HASH_CRC,
  HASHKIT_HASH_FNV1_64,
  HASHKIT_HASH_FNV1A_64,
  HASHKIT_HASH_FNV1_32,
  HASHKIT_HASH_FNV1A_32,
  HASHKIT_HASH_HSIEH,
  HASHKIT_HASH_MURMUR,
  HASHKIT_HASH_JENKINS,
  HASHKIT_HASH_MURMUR3,
  HASHKIT_HASH_CUSTOM,
  HASHKIT_HASH_MAX
} hashkit_hash_algorithm_t;

/**
 * Hash distributions that are available to use.
 */
typedef enum {
  HASHKIT_DISTRIBUTION_MODULA,
  HASHKIT_DISTRIBUTION_RANDOM,
  HASHKIT_DISTRIBUTION_KETAMA,
  HASHKIT_DISTRIBUTION_MAX /* Always add new values before this. */
} hashkit_distribution_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hashkit_st hashkit_st;
typedef struct hashkit_string_st hashkit_string_st;

typedef uint32_t (*hashkit_hash_fn)(const char *key, size_t key_length, void *context);

#ifdef __cplusplus
}
#endif
