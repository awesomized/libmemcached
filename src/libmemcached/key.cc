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

static inline memcached_return_t memcached_validate_key_length(size_t key_length, bool) {
  if (key_length == 0) {
    return MEMCACHED_BAD_KEY_PROVIDED;
  }

  // No one ever reimplemented MEMCACHED to use keys longer then the original ascii length
#if 0
  if (binary)
  {
    if (key_length > 0xffff)
    {
      return MEMCACHED_BAD_KEY_PROVIDED;
    }
  }
  else
#endif
  {
    if (key_length >= MEMCACHED_MAX_KEY) {
      return MEMCACHED_BAD_KEY_PROVIDED;
    }
  }

  return MEMCACHED_SUCCESS;
}

memcached_return_t memcached_key_test(memcached_st &memc, const char *const *keys,
                                      const size_t *key_length, size_t number_of_keys) {
  if (number_of_keys == 0) {
    return memcached_set_error(memc, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
                               memcached_literal_param("Numbers of keys provided was zero"));
  }

  if (keys == NULL or key_length == NULL) {
    return memcached_set_error(memc, MEMCACHED_BAD_KEY_PROVIDED, MEMCACHED_AT,
                               memcached_literal_param("Key was NULL or length of key was zero."));
  }

  const bool is_binary = memcached_flag(memc, MEMCACHED_FLAG_BINARY_PROTOCOL);

  // If we don't need to verify the key, or we are using the binary protoocol,
  // we just check the size of the key
  for (size_t x = 0; x < number_of_keys; ++x) {
    // We should set binary key, but the memcached server is broken for
    // longer keys at the moment.
    memcached_return_t rc =
        memcached_validate_key_length(*(key_length + x), false /* memc.flags.binary_protocol */);
    if (memcached_failed(rc)) {
      return memcached_set_error(memc, rc, MEMCACHED_AT,
                                 memcached_literal_param("Key provided was too long."));
    }

    if (memc.flags.verify_key and is_binary == false) {
      for (size_t y = 0; y < *(key_length + x); ++y) {
        if ((isgraph(keys[x][y])) == 0) {
          return memcached_set_error(
              memc, MEMCACHED_BAD_KEY_PROVIDED, MEMCACHED_AT,
              memcached_literal_param("Key provided had invalid character."));
        }
      }
    }
  }

  return MEMCACHED_SUCCESS;
}
