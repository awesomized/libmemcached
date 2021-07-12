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

hashkit_string_st *hashkit_encrypt(hashkit_st *kit, const char *source, size_t source_length) {
  return aes_encrypt((aes_key_t *) kit->_key, source,
                     source_length);
}

hashkit_string_st *hashkit_decrypt(hashkit_st *kit, const char *source, size_t source_length) {
  return aes_decrypt((aes_key_t *)kit->_key, source, source_length);
}

bool hashkit_key(hashkit_st *kit, const char *key, const size_t key_length) {
  if (kit->_key) {
    aes_free_key((aes_key_t *) kit->_key);
  }

  kit->_key = aes_create_key(key, key_length);

  return bool(kit->_key);
}
