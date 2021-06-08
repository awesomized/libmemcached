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

bool libhashkit_has_algorithm(const hashkit_hash_algorithm_t algo) {
  switch (algo) {
  case HASHKIT_HASH_FNV1_64:
  case HASHKIT_HASH_FNV1A_64:
#if __WORDSIZE == 64 && defined(HAVE_FNV64_HASH)
    return true;
#else
    return false;
#endif

  case HASHKIT_HASH_HSIEH:
#ifdef HAVE_HSIEH_HASH
    return true;
#else
    return false;
#endif

  case HASHKIT_HASH_MURMUR3:
  case HASHKIT_HASH_MURMUR:
#ifdef HAVE_MURMUR_HASH
    return true;
#else
    return false;
#endif

  case HASHKIT_HASH_FNV1_32:
  case HASHKIT_HASH_FNV1A_32:
  case HASHKIT_HASH_DEFAULT:
  case HASHKIT_HASH_MD5:
  case HASHKIT_HASH_CRC:
  case HASHKIT_HASH_JENKINS:
  case HASHKIT_HASH_CUSTOM:
    return true;

  case HASHKIT_HASH_MAX:
    break;
  }

  return false;
}
