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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* The two public hash bits */
LIBMEMCACHED_API
uint32_t memcached_generate_hash_value(const char *key, size_t key_length,
                                       memcached_hash_t hash_algorithm);

LIBMEMCACHED_API
const hashkit_st *memcached_get_hashkit(const memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_set_hashkit(memcached_st *ptr, hashkit_st *hashk);

LIBMEMCACHED_API
uint32_t memcached_generate_hash(const memcached_st *ptr, const char *key, size_t key_length);

LIBMEMCACHED_API
void memcached_autoeject(memcached_st *ptr);

LIBMEMCACHED_API
const char *libmemcached_string_hash(memcached_hash_t type);

#ifdef __cplusplus
}
#endif
