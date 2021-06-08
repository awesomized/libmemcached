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

#include "libmemcached/common.h"
#include "libmemcached/assert.hpp"

memcached_return_t memcached_set_namespace(Memcached &memc, const char *key, size_t key_length) {
  if (key and key_length == 0) {
    WATCHPOINT_ASSERT(key_length);
    return memcached_set_error(
        memc, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
        memcached_literal_param("Invalid namespace, namespace string had value but length was 0"));
  } else if (key_length and key == NULL) {
    WATCHPOINT_ASSERT(key);
    return memcached_set_error(
        memc, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
        memcached_literal_param(
            "Invalid namespace, namespace string length was > 1 but namespace string was null "));
  } else if (key and key_length) {
    bool orig = memc.flags.verify_key;
    memc.flags.verify_key = true;
    if (memcached_failed(memcached_key_test(memc, (const char **) &key, &key_length, 1))) {
      memc.flags.verify_key = orig;
      return memcached_last_error(&memc);
    }
    memc.flags.verify_key = orig;

    if ((key_length > MEMCACHED_MAX_NAMESPACE - 1)) {
      return memcached_set_error(memc, MEMCACHED_KEY_TOO_BIG, MEMCACHED_AT);
    }

    memcached_array_free(memc._namespace);
    memc._namespace = memcached_strcpy(&memc, key, key_length);

    if (memc._namespace == NULL) {
      return memcached_set_error(memc, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT);
    }
  } else {
    memcached_array_free(memc._namespace);
    memc._namespace = NULL;
  }

  return MEMCACHED_SUCCESS;
}

const char *memcached_get_namespace(Memcached &memc) {
  if (memc._namespace == NULL) {
    return NULL;
  }

  return memcached_array_string(memc._namespace);
}
