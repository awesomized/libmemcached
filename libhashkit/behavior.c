/* HashKit
 * Copyright (C) 2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include "common.h"

static hashkit_fn *fetch_hash_fn(hashkit_hash_algorithm_t hash_algorithm)
{
  switch (hash_algorithm)
  {
  case HASHKIT_HASH_DEFAULT:
    return hashkit_default;
  case HASHKIT_HASH_MD5:
    return hashkit_md5;
  case HASHKIT_HASH_CRC:
    return hashkit_crc32;
  case HASHKIT_HASH_FNV1_64:
    return hashkit_fnv1_64;
  case HASHKIT_HASH_FNV1A_64:
    return hashkit_fnv1a_64;
  case HASHKIT_HASH_FNV1_32:
    return hashkit_fnv1_32;
  case HASHKIT_HASH_FNV1A_32:
    return hashkit_fnv1a_32;
  case HASHKIT_HASH_HSIEH:
#ifdef HAVE_HSIEH_HASH
    return hashkit_hsieh;
#else
    return NULL;
#endif
  case HASHKIT_HASH_MURMUR:
    return hashkit_murmur;
  case HASHKIT_HASH_JENKINS:
    return hashkit_jenkins;
  case HASHKIT_HASH_MAX:
  default:
#ifdef HAVE_DEBUG
    fprintf(stderr, "hashkit_hash_t was extended but hashkit_generate_value was not updated\n");
    fflush(stderr);
    assert(0);
#endif
    break;
  }

  return NULL;
}

hashkit_return_t hashkit_behavior_set_distribution(hashkit_st *hashkit, hashkit_distribution_t distribution)
{
  hashkit->distribution= distribution;

  return HASHKIT_SUCCESS;
}


hashkit_distribution_t hashkit_behavior_get_distribution(hashkit_st *hashkit)
{
  return hashkit->distribution;
}


/**
  @note For the moment we will not allow the user to set the distribution hash type.
*/
hashkit_return_t hashkit_behavior_set_key_hash_algorithm(hashkit_st *hashkit, hashkit_hash_algorithm_t hash_algorithm)
{
  hashkit_fn *hash_fn= fetch_hash_fn(hash_algorithm);

  if (hash_fn == NULL)
    return HASHKIT_FAILURE;

  hashkit->hash_fn= hash_fn;
  hashkit->for_key= hash_algorithm;
  hashkit->for_distribution= hash_algorithm;
  
  return HASHKIT_SUCCESS;
}


hashkit_hash_algorithm_t hashkit_behavior_get_key_hash_algorithm(hashkit_st *hashkit)
{
  return hashkit->for_key;
}


void hashkit_behavior_set_active_fn(hashkit_st *hashkit, hashkit_active_fn *function)
{
  hashkit->active_fn= function;
}


hashkit_active_fn * hashkit_behavior_get_active_fn(hashkit_st *hashkit)
{
  return hashkit->active_fn;
}


void hashkit_behavior_set_continuum_hash_fn(hashkit_st *hashkit, hashkit_fn *function)
{
  hashkit->continuum_hash_fn= function;
}


hashkit_fn * hashkit_behavior_get_continuum_hash_fn(hashkit_st *hashkit)
{
  return hashkit->continuum_hash_fn;
}


void hashkit_behavior_set_continuum_key_fn(hashkit_st *hashkit, hashkit_key_fn *function)
{
  hashkit->continuum_key_fn= function;
}


hashkit_key_fn * hashkit_behavior_get_continuum_key_fn(hashkit_st *hashkit)
{
  return hashkit->continuum_key_fn;
}


void hashkit_behavior_set_sort_fn(hashkit_st *hashkit, hashkit_sort_fn *function)
{
  hashkit->sort_fn= function;
}


hashkit_sort_fn * hashkit_behavior_get_sort_fn(hashkit_st *hashkit)
{
  return hashkit->sort_fn;
}


void hashkit_behavior_set_weight_fn(hashkit_st *hashkit, hashkit_weight_fn *function)
{
  hashkit->weight_fn= function;
}


hashkit_weight_fn * hashkit_behavior_get_weight_fn(hashkit_st *hashkit)
{
  return hashkit->weight_fn;
}
