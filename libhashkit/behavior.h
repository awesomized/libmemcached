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

#ifndef HASHKIT_BEHAVIOR_H
#define HASHKIT_BEHAVIORH

#ifdef __cplusplus
extern "C" {
#endif


HASHKIT_API
hashkit_return_t hashkit_behavior_set_distribution(hashkit_st *hashkit, hashkit_distribution_t distribution);

HASHKIT_API
hashkit_distribution_t hashkit_behavior_get_distribution(hashkit_st *hashkit);

HASHKIT_API
hashkit_return_t hashkit_behavior_set_key_hash_algorithm(hashkit_st *hashkit, hashkit_hash_algorithm_t hash_algorithm);

HASHKIT_API
hashkit_hash_algorithm_t hashkit_behavior_get_key_hash_algorithm(hashkit_st *hashkit);

HASHKIT_API
void hashkit_behavior_set_active_fn(hashkit_st *hash, hashkit_active_fn *function);

HASHKIT_API
hashkit_active_fn * hashkit_behavior_get_active_fn(hashkit_st *hash);

HASHKIT_API
void hashkit_behavior_set_continuum_hash_fn(hashkit_st *hash, hashkit_fn *function);

HASHKIT_API
hashkit_fn * hashkit_behavior_get_continuum_hash_fn(hashkit_st *hash);

HASHKIT_API
void hashkit_behavior_set_continuum_key_fn(hashkit_st *hash, hashkit_key_fn *function);

HASHKIT_API
hashkit_key_fn * hashkit_behavior_get_continuum_key_fn(hashkit_st *hash);

HASHKIT_API
void hashkit_behavior_set_sort_fn(hashkit_st *hash, hashkit_sort_fn *function);

HASHKIT_API
hashkit_sort_fn * hashkit_behavior_get_sort_fn(hashkit_st *hash);

HASHKIT_API
void hashkit_behavior_set_weight_fn(hashkit_st *hash, hashkit_weight_fn *function);

HASHKIT_API
hashkit_weight_fn * hashkit_behavior_get_weight_fn(hashkit_st *hash);

#ifdef __cplusplus
}
#endif

#endif /* HASHKIT_BEHAVIOR_H */
