
/* HashKit
 * Copyright (C) 2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief HashKit Header
 */

#ifndef HASHKIT_TYPES_H
#define HASHKIT_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup hashkit_types Types
 * @ingroup hashkit
 * @{
 */

typedef enum {
  HASHKIT_SUCCESS,
  HASHKIT_FAILURE,
  HASHKIT_MEMORY_ALLOCATION_FAILURE,
  HASHKIT_MAXIMUM_RETURN /* Always add new error code before */
} hashkit_return_t;

/**
  @todo hashkit_options_t is for future use, currently we do not define any user options.
 */

typedef enum
{
  HASHKIT_OPTION_MAX
} hashkit_options_t;

typedef struct
{
  /* We use the following for internal book keeping. */
  bool is_initialized:1;
  bool is_allocated:1;
} hashkit_options_st;

typedef enum {
  HASHKIT_HASH_DEFAULT= 0,
  HASHKIT_HASH_MD5,
  HASHKIT_HASH_CRC,
  HASHKIT_HASH_FNV1_64,
  HASHKIT_HASH_FNV1A_64,
  HASHKIT_HASH_FNV1_32,
  HASHKIT_HASH_FNV1A_32,
  HASHKIT_HASH_HSIEH,
  HASHKIT_HASH_MURMUR,
  HASHKIT_HASH_JENKINS,
  HASHKIT_HASH_MAX
} hashkit_hash_algorithm_t;

/**
 * Hash distributions that are available to use.
 */
typedef enum
{
  HASHKIT_DISTRIBUTION_MODULA,
  HASHKIT_DISTRIBUTION_RANDOM,
  HASHKIT_DISTRIBUTION_KETAMA,
  HASHKIT_DISTRIBUTION_MAX /* Always add new values before this. */
} hashkit_distribution_t;


typedef struct hashkit_st hashkit_st;
typedef struct hashkit_continuum_point_st hashkit_continuum_point_st;
typedef bool (hashkit_active_fn)(void *context);
typedef uint32_t (hashkit_fn)(const char *key, size_t key_length);
typedef size_t (hashkit_key_fn)(char *key, size_t key_length, uint32_t point_index, void *context);
typedef void (hashkit_sort_fn)(void *context, size_t count);
typedef uint32_t (hashkit_weight_fn)(void *context);

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* HASHKIT_TYPES_H */
