/* HashKit
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include "common.h"

inline static bool _is_allocated(const hashkit_st *hashk)
{
  return hashk->options.is_allocated == true;
}

inline static bool _is_initialized(const hashkit_st *hashk)
{
  return hashk->options.is_initialized == true;
}

/**
  @note We make no assumptions that "hashk" has been, or not been allocated from heap/stack. We just know we didn't do it.
*/
hashkit_st *hashkit_create(hashkit_st *hashk)
{
  if (hashk == NULL)
  {
    hashk= (hashkit_st *)malloc(sizeof(hashkit_st));
    if (hashk == NULL)
    {
      return NULL;
    }

    hashk->options.is_allocated= true;
  }
  else
  {
    hashk->options.is_allocated= false;
  }

  hashk->options.is_initialized= true;

  hashk->distribution= HASHKIT_DISTRIBUTION_MODULA;
  hashk->continuum_count= 0;
  hashk->continuum_points_count= 0;
  hashk->list_size= 0;
  hashk->context_size= 0;
  hashk->continuum= NULL;
  hashk->hash_fn= NULL;
  hashk->active_fn= NULL;
  hashk->continuum_hash_fn= NULL;
  hashk->continuum_key_fn= NULL;
  hashk->sort_fn= NULL;
  hashk->weight_fn= NULL;
  hashk->list= NULL;

  return hashk;
}


void hashkit_free(hashkit_st *hashk)
{
  assert(_is_initialized(hashk) == true);

  if (hashk->continuum != NULL)
  {
    free(hashk->continuum);
  }

  /**
    We don't know if hashk is pointing to something else,
    so we go on and set is_initialized.
  */
  hashk->options.is_initialized= false;

  if (_is_allocated(hashk))
  {
    free(hashk);
  }
}

/**
  @note We do assume source is valid. If source does not exist, we allocate.
*/
hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *source)
{
  hashkit_st *new_clone;

  if (source == NULL)
  {
    return hashkit_create(destination);
  }
  else
  {
    assert(_is_initialized(source) == true);
  }

  /* new_clone will be a pointer to destination */ 
  new_clone= hashkit_create(destination);
  assert((destination ?  ((_is_allocated(new_clone) == false)) : (_is_allocated(new_clone) == true)));

  // Should only happen on allocation failure.
  if (new_clone == NULL)
  {
    return NULL;
  }

  // For the moment we will not clone this.
  new_clone->continuum= NULL;

  new_clone->distribution= source->distribution;
  new_clone->continuum_count= source->continuum_count;
  new_clone->continuum_points_count= source->continuum_points_count;
  new_clone->list_size= source->list_size;
  new_clone->context_size= source->context_size;


  new_clone->hash_fn= source->hash_fn;
  new_clone->active_fn= source->active_fn;
  new_clone->continuum_hash_fn= source->continuum_hash_fn;
  new_clone->continuum_key_fn= source->continuum_key_fn;
  new_clone->sort_fn= source->sort_fn;
  new_clone->weight_fn= source->weight_fn;
  new_clone->list= source->list;

  return new_clone;
}


#if 0
void hashkit_set_list(hashkit_st *hashkit, void *list, size_t list_size, size_t context_size)
{
  hashkit->list= list;
  hashkit->list_size= list_size;
  hashkit->context_size= context_size;
}


uint32_t hashkit_value(hashkit_st *hashkit, const char *key, size_t key_length)
{
  if (hashkit->hash_fn == NULL)
    return hashkit_default(key, key_length);

  return hashkit->hash_fn(key, key_length);
}


uint32_t hashkit_index(hashkit_st *hashkit, uint32_t hash_value)
{
  if (hashkit->list_size == 1)
    return 0;

  switch (hashkit->distribution)
  {
  case HASHKIT_DISTRIBUTION_MODULA:
    return hash_value % (uint32_t)hashkit->list_size;

  case HASHKIT_DISTRIBUTION_RANDOM:
    return (uint32_t)random() % (uint32_t)hashkit->list_size;

  case HASHKIT_DISTRIBUTION_KETAMA:
  {
    hashkit_continuum_point_st *begin, *end, *left, *right, *middle;
    begin= left= hashkit->continuum;
    end= right= hashkit->continuum + hashkit->continuum_points_count;

    while (left < right)
    {
      middle= left + (right - left) / 2;
      if (middle->value < hash_value)
        left= middle + 1;
      else
        right= middle;
    }
    if (right == end)
      right= begin;
    return right->index;
  }

  case HASHKIT_DISTRIBUTION_MAX:
  default:
    /* We have added a distribution without extending the logic */
    return hash_value % (uint32_t)hashkit->list_size;
  }

  /* NOTREACHED */
}


int hashkit_run_distribution(hashkit_st *hashkit)
{
  switch (hashkit->distribution)
  {
  case HASHKIT_DISTRIBUTION_MODULA:
    if (hashkit->sort_fn != NULL && hashkit->list_size > 1)
      hashkit->sort_fn(hashkit->list, hashkit->list_size);
    break;
  case HASHKIT_DISTRIBUTION_RANDOM:
    break;
  case HASHKIT_DISTRIBUTION_KETAMA:
    return update_continuum(hashkit);
  case HASHKIT_DISTRIBUTION_MAX:
  default:
    /* We have added a distribution without extending the logic */
    break;
  }

  return 0;
}
#endif


uint32_t hashkit_generate_value(const char *key, size_t key_length, hashkit_hash_algorithm_t hash_algorithm)
{
  switch (hash_algorithm)
  {
  case HASHKIT_HASH_DEFAULT:
    return hashkit_default(key, key_length);
  case HASHKIT_HASH_MD5:
    return hashkit_md5(key, key_length);
  case HASHKIT_HASH_CRC:
    return hashkit_crc32(key, key_length);
  case HASHKIT_HASH_FNV1_64:
    return hashkit_fnv1_64(key, key_length);
  case HASHKIT_HASH_FNV1A_64:
    return hashkit_fnv1a_64(key, key_length);
  case HASHKIT_HASH_FNV1_32:
    return hashkit_fnv1_32(key, key_length);
  case HASHKIT_HASH_FNV1A_32:
    return hashkit_fnv1a_32(key, key_length);
  case HASHKIT_HASH_HSIEH:
#ifdef HAVE_HSIEH_HASH
    return hashkit_hsieh(key, key_length);
#else
    return 1;
#endif
  case HASHKIT_HASH_MURMUR:
    return hashkit_murmur(key, key_length);
  case HASHKIT_HASH_JENKINS:
    return hashkit_jenkins(key, key_length);
  case HASHKIT_HASH_MAX:
  default:
#ifdef HAVE_DEBUG
    fprintf(stderr, "hashkit_hash_t was extended but hashkit_generate_value was not updated\n");
    fflush(stderr);
    assert(0);
#endif
    break;
  }

  return 1;
}
