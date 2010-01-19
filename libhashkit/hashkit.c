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
    .type= HASHKIT_HASH_DEFAULT,
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

/**
  @note We do assume source is valid. If source does not exist, we allocate.
*/
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


uint32_t hashkit_generate_value(const hashkit_st *self, const char *key, size_t key_length)
{
  return self->base_hash.function(key, key_length);
}

uint32_t libhashkit_generate_value(const char *key, size_t key_length, hashkit_hash_algorithm_t hash_algorithm)
{
  switch (hash_algorithm)
  {
  case HASHKIT_HASH_DEFAULT:
    return hashkit_one_at_a_time(key, key_length);
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
    fprintf(stderr, "hashkit_hash_t was extended but libhashkit_generate_value was not updated\n");
    fflush(stderr);
    assert(0);
#endif
    break;
  }

  return 1;
}
