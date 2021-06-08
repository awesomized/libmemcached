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

const char *hashkit_strerror(hashkit_st *ptr, hashkit_return_t rc) {
  (void) ptr;
  switch (rc) {
  case HASHKIT_SUCCESS:
    return "SUCCESS";
  case HASHKIT_FAILURE:
    return "FAILURE";
  case HASHKIT_MEMORY_ALLOCATION_FAILURE:
    return "MEMORY ALLOCATION FAILURE";
  case HASHKIT_INVALID_ARGUMENT:
    return "INVALID ARGUMENT";
  case HASHKIT_INVALID_HASH:
    return "INVALID hashkit_hash_algorithm_t";
  case HASHKIT_MAXIMUM_RETURN:
  default:
    return "INVALID hashkit_return_t";
  }
}
