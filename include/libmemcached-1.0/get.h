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

/* Public defines */
LIBMEMCACHED_API
char *memcached_get(memcached_st *ptr, const char *key, size_t key_length, size_t *value_length,
                    uint32_t *flags, memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_mget(memcached_st *ptr, const char *const *keys,
                                  const size_t *key_length, size_t number_of_keys);

LIBMEMCACHED_API
char *memcached_get_by_key(memcached_st *ptr, const char *group_key, size_t group_key_length,
                           const char *key, size_t key_length, size_t *value_length,
                           uint32_t *flags, memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_mget_by_key(memcached_st *ptr, const char *group_key,
                                         size_t group_key_length, const char *const *keys,
                                         const size_t *key_length, const size_t number_of_keys);

LIBMEMCACHED_API
char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, size_t *value_length,
                      uint32_t *flags, memcached_return_t *error);

LIBMEMCACHED_API
memcached_result_st *memcached_fetch_result(memcached_st *ptr, memcached_result_st *result,
                                            memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_mget_execute(memcached_st *ptr, const char *const *keys,
                                          const size_t *key_length, const size_t number_of_keys,
                                          memcached_execute_fn *callback, void *context,
                                          const uint32_t number_of_callbacks);

LIBMEMCACHED_API
memcached_return_t memcached_mget_execute_by_key(memcached_st *ptr, const char *group_key,
                                                 size_t group_key_length, const char *const *keys,
                                                 const size_t *key_length, size_t number_of_keys,
                                                 memcached_execute_fn *callback, void *context,
                                                 const uint32_t number_of_callbacks);

#ifdef __cplusplus
}
#endif
