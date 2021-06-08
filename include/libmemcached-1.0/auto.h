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

LIBMEMCACHED_API
memcached_return_t memcached_increment(memcached_st *ptr, const char *key, size_t key_length,
                                       uint32_t offset, uint64_t *value);
LIBMEMCACHED_API
memcached_return_t memcached_decrement(memcached_st *ptr, const char *key, size_t key_length,
                                       uint32_t offset, uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_increment_by_key(memcached_st *ptr, const char *group_key,
                                              size_t group_key_length, const char *key,
                                              size_t key_length, uint64_t offset, uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_decrement_by_key(memcached_st *ptr, const char *group_key,
                                              size_t group_key_length, const char *key,
                                              size_t key_length, uint64_t offset, uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_increment_with_initial(memcached_st *ptr, const char *key,
                                                    size_t key_length, uint64_t offset,
                                                    uint64_t initial, time_t expiration,
                                                    uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_decrement_with_initial(memcached_st *ptr, const char *key,
                                                    size_t key_length, uint64_t offset,
                                                    uint64_t initial, time_t expiration,
                                                    uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_increment_with_initial_by_key(memcached_st *ptr, const char *group_key,
                                                           size_t group_key_length, const char *key,
                                                           size_t key_length, uint64_t offset,
                                                           uint64_t initial, time_t expiration,
                                                           uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_decrement_with_initial_by_key(memcached_st *ptr, const char *group_key,
                                                           size_t group_key_length, const char *key,
                                                           size_t key_length, uint64_t offset,
                                                           uint64_t initial, time_t expiration,
                                                           uint64_t *value);

#ifdef __cplusplus
}
#endif
