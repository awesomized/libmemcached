/* HashKit
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include "common.h"

static const hashkit_st global_default_hash= {
  .base_hash= {
    .function= hashkit_one_at_a_time,
    .context= NULL
  },
};

/**
  @note We make no assumptions that "hashk" has been, or not been allocated from heap/stack. We just know we didn't do it.
*/
hashkit_st *hashkit_create(hashkit_st *self)
{
  if (self == NULL)
  {
    self= (hashkit_st *)malloc(sizeof(hashkit_st));
    if (self == NULL)
    {
      return NULL;
    }

    self->options.is_allocated= true;
  }
  else
  {
    self->options.is_allocated= false;
  }

  self->base_hash= global_default_hash.base_hash;

  return self;
}


void hashkit_free(hashkit_st *self)
{
  if (hashkit_is_allocated(self))
  {
    free(self);
  }
}

hashkit_st *hashkit_clone(hashkit_st *destination, const hashkit_st *source)
{
  if (source == NULL)
  {
    return hashkit_create(destination);
  }

  /* new_clone will be a pointer to destination */ 
  destination= hashkit_create(destination);

  // Should only happen on allocation failure.
  if (destination == NULL)
  {
    return NULL;
  }

  destination->base_hash= source->base_hash;

  return destination;
}

bool hashkit_compare(const hashkit_st *first, const hashkit_st *second)
{
  if (first->base_hash.function == second->base_hash.function &&
      first->base_hash.context == second->base_hash.context)
    return true;

  return false;
}

uint32_t hashkit_generate_value(const hashkit_st *self, const char *key, size_t key_length)
{
  return self->base_hash.function(key, key_length, self->base_hash.context);
}

hashkit_return_t hashkit_set_base_function(hashkit_st *self, hashkit_hash_algorithm_t hash_algorithm)
{
  switch (hash_algorithm)
  {
  case HASHKIT_HASH_DEFAULT:
    self->base_hash.function= hashkit_one_at_a_time;
    break;
  case HASHKIT_HASH_MD5:
    self->base_hash.function= hashkit_md5;
    break;
  case HASHKIT_HASH_CRC:
    self->base_hash.function= hashkit_crc32;
    break;
  case HASHKIT_HASH_FNV1_64:
    self->base_hash.function= hashkit_fnv1_64;
    break;
  case HASHKIT_HASH_FNV1A_64:
    self->base_hash.function= hashkit_fnv1a_64;
    break;
  case HASHKIT_HASH_FNV1_32:
    self->base_hash.function= hashkit_fnv1_32;
    break;
  case HASHKIT_HASH_FNV1A_32:
    self->base_hash.function= hashkit_fnv1a_32;
    break;
  case HASHKIT_HASH_HSIEH:
#ifdef HAVE_HSIEH_HASH
    self->base_hash.function= hashkit_hsieh;
    break;    
#else
    return HASHKIT_FAILURE;
#endif
  case HASHKIT_HASH_MURMUR:
    self->base_hash.function= hashkit_murmur;
    break;    
  case HASHKIT_HASH_JENKINS:
    self->base_hash.function= hashkit_jenkins;
    break;    
  case HASHKIT_HASH_CUSTOM:
  case HASHKIT_HASH_MAX:
  default:
    return HASHKIT_FAILURE;
    break;
  }

  self->base_hash.context= NULL;

  return HASHKIT_SUCCESS;
}

hashkit_return_t hashkit_set_base_function_custom(hashkit_st *self, hashkit_hash_fn function, void *context)
{
  if (function)
  {
    self->base_hash.function= function;
    self->base_hash.context= context;

    return HASHKIT_SUCCESS;
  }

  return HASHKIT_FAILURE;
}

hashkit_hash_algorithm_t hashkit_get_base_function(const hashkit_st *self)
{
  if (self->base_hash.function == hashkit_one_at_a_time)
  {
    return HASHKIT_HASH_DEFAULT;
  }
  else if (self->base_hash.function == hashkit_md5)
  {
    return HASHKIT_HASH_MD5;
  }
  else if (self->base_hash.function == hashkit_crc32)
  {
    return HASHKIT_HASH_CRC;
  }
  else if (self->base_hash.function == hashkit_fnv1_64)
  {
    return HASHKIT_HASH_FNV1_64;
  }
  else if (self->base_hash.function == hashkit_fnv1a_64)
  {
    return HASHKIT_HASH_FNV1A_64;
  }
  else if (self->base_hash.function == hashkit_fnv1_32)
  {
    return HASHKIT_HASH_FNV1_32;
  }
  else if (self->base_hash.function == hashkit_fnv1a_32)
  {
    return HASHKIT_HASH_FNV1A_32;
  }
#ifdef HAVE_HSIEH_HASH
  else if (self->base_hash.function == hashkit_hsieh)
  {
    return HASHKIT_HASH_HSIEH;
  }
#endif
  else if (self->base_hash.function == hashkit_murmur)
  {
    return HASHKIT_HASH_MURMUR;
  }
  else if (self->base_hash.function == hashkit_jenkins)
  {
    return HASHKIT_HASH_JENKINS;
  }

  return HASHKIT_HASH_CUSTOM;
}

uint32_t libhashkit_generate_value(const char *key, size_t key_length, hashkit_hash_algorithm_t hash_algorithm)
{
  switch (hash_algorithm)
  {
  case HASHKIT_HASH_DEFAULT:
    return libhashkit_one_at_a_time(key, key_length);
  case HASHKIT_HASH_MD5:
    return libhashkit_md5(key, key_length);
  case HASHKIT_HASH_CRC:
    return libhashkit_crc32(key, key_length);
  case HASHKIT_HASH_FNV1_64:
    return libhashkit_fnv1_64(key, key_length);
  case HASHKIT_HASH_FNV1A_64:
    return libhashkit_fnv1a_64(key, key_length);
  case HASHKIT_HASH_FNV1_32:
    return libhashkit_fnv1_32(key, key_length);
  case HASHKIT_HASH_FNV1A_32:
    return libhashkit_fnv1a_32(key, key_length);
  case HASHKIT_HASH_HSIEH:
#ifdef HAVE_HSIEH_HASH
    return libhashkit_hsieh(key, key_length);
#else
    return 1;
#endif
  case HASHKIT_HASH_MURMUR:
    return libhashkit_murmur(key, key_length);
  case HASHKIT_HASH_JENKINS:
    return libhashkit_jenkins(key, key_length);
  case HASHKIT_HASH_CUSTOM:
  case HASHKIT_HASH_MAX:
  default:
#ifdef HAVE_DEBUG
    fprintf(stderr, "hashkit_hash_t was extended but libhashkit_generate_value was not updated\n");
    fflush(stderr);
    assert(0);
#endif
    break;
  }

  return 1;
}
