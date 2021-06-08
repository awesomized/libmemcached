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

const char *libhashkit_string_hash(hashkit_hash_algorithm_t type) {
  switch (type) {
  case HASHKIT_HASH_DEFAULT:
    return "DEFAULT";
  case HASHKIT_HASH_MD5:
    return "MD5";
  case HASHKIT_HASH_CRC:
    return "CRC";
  case HASHKIT_HASH_FNV1_64:
    return "FNV1_64";
  case HASHKIT_HASH_FNV1A_64:
    return "FNV1A_64";
  case HASHKIT_HASH_FNV1_32:
    return "FNV1_32";
  case HASHKIT_HASH_FNV1A_32:
    return "FNV1A_32";
  case HASHKIT_HASH_HSIEH:
    return "HSIEH";
  case HASHKIT_HASH_MURMUR:
    return "MURMUR";
  case HASHKIT_HASH_MURMUR3:
    return "MURMUR3";
  case HASHKIT_HASH_JENKINS:
    return "JENKINS";
  case HASHKIT_HASH_CUSTOM:
    return "CUSTOM";
  default:
  case HASHKIT_HASH_MAX:
    return "INVALID";
  }
}
