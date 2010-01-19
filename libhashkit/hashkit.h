/* HashKit
 * Copyright (C) 2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
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

struct hashkit_st
{
  struct {
    hashkit_hash_algorithm_t type;
    hashkit_hash_fn function;
    void *context;
  } base_hash;

  struct {
    bool is_allocated:1;
  } options;
};

HASHKIT_API
hashkit_st *hashkit_create(hashkit_st *hash);

HASHKIT_API
hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *ptr);

HASHKIT_API
void hashkit_free(hashkit_st *hash);

HASHKIT_API
uint32_t hashkit_generate_value(const hashkit_st *self, const char *key, size_t key_length);

HASHKIT_API
uint32_t libhashkit_generate_value(const char *key, size_t key_length, hashkit_hash_algorithm_t hash_algorithm);

#define hashkit_is_allocated(__object) ((__object)->options.is_allocated)
#define hashkit_is_initialized(__object) ((__object)->options.is_initialized)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* HASHKIT_H */
