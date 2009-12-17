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

#ifndef HASHKIT_H
#define HASHKIT_H

#if !defined(__cplusplus)
# include <stdbool.h>
#endif
#include <inttypes.h>
#include <sys/types.h>
#include <libhashkit/visibility.h>
#include <libhashkit/types.h>
#include <libhashkit/algorithm.h>
#include <libhashkit/behavior.h>
#include <libhashkit/strerror.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup hashkit_constants Constants
 * @ingroup hashkit
 * @{
 */

/* Defines. */
#define HASHKIT_MAX_KEY 251
#define HASHKIT_POINTS_PER_NODE 100
#define HASHKIT_POINTS_PER_NODE_WEIGHTED 160
#define HASHKIT_CONTINUUM_ADDITION 10
#define HASHKIT_CONTINUUM_KEY_SIZE 86

/** @} */

/**
 * @ingroup hashkit
 */
struct hashkit_st
{
  hashkit_options_st options;
  hashkit_distribution_t distribution;
  uint32_t continuum_count;
  uint32_t continuum_points_count;
  size_t list_size;
  size_t context_size;

  /**
    @note There are two places we use hashing, one is for when we have a key
    and we want to find out what server it should be placed on. The second is
    for when we are placing a value into the continuum.
  */
  hashkit_hash_algorithm_t for_key;
  hashkit_hash_algorithm_t for_distribution;

  hashkit_continuum_point_st *continuum;
  hashkit_fn *hash_fn;
  hashkit_active_fn *active_fn;
  hashkit_fn *continuum_hash_fn;
  hashkit_key_fn *continuum_key_fn;
  hashkit_sort_fn *sort_fn;
  hashkit_weight_fn *weight_fn;
  void *list;
};

/**
 * @ingroup hashkit
 */
struct hashkit_continuum_point_st
{
  uint32_t index;
  uint32_t value;
};

/**
 * @addtogroup hashkit Pandora Hash Declarations
 * @{
 */

HASHKIT_API
hashkit_st *hashkit_create(hashkit_st *hash);

HASHKIT_API
hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *ptr);

HASHKIT_API
void hashkit_free(hashkit_st *hash);

HASHKIT_API
uint32_t hashkit_generate_value(const char *key, size_t key_length, hashkit_hash_algorithm_t hash_algorithm);

#define hashkit_is_allocated(__object) ((__object)->options.is_allocated)
#define hashkit_is_initialized(__object) ((__object)->options.is_initialized)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HASHKIT_H */
