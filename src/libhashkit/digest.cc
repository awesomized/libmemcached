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

#include "libhashkit/common.h"

uint32_t hashkit_digest(const hashkit_st *self, const char *key, size_t key_length) {
  return self->base_hash.function(key, key_length, self->base_hash.context);
}

uint32_t libhashkit_digest(const char *key, size_t key_length,
                           hashkit_hash_algorithm_t hash_algorithm) {
  switch (hash_algorithm) {
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
  case HASHKIT_HASH_MURMUR3:
    return libhashkit_murmur3(key, key_length);

  case HASHKIT_HASH_MURMUR:
#ifdef HAVE_MURMUR_HASH
    return libhashkit_murmur(key, key_length);
#else
    return 1;
#endif
  case HASHKIT_HASH_JENKINS:
    return libhashkit_jenkins(key, key_length);
  case HASHKIT_HASH_CUSTOM:
  case HASHKIT_HASH_MAX:
  default:
    if (DEBUG) {
      fprintf(stderr,
              "hashkit_hash_t was extended but libhashkit_generate_value was not updated\n");
      fflush(stderr);
      assert(0);
    }
    break;
  }

  return 1;
}
