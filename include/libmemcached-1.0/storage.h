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

/* All of the functions for adding data to the server */
LIBMEMCACHED_API
memcached_return_t memcached_set(memcached_st *ptr, const char *key, size_t key_length,
                                 const char *value, size_t value_length, time_t expiration,
                                 uint32_t flags);
LIBMEMCACHED_API
memcached_return_t memcached_add(memcached_st *ptr, const char *key, size_t key_length,
                                 const char *value, size_t value_length, time_t expiration,
                                 uint32_t flags);
LIBMEMCACHED_API
memcached_return_t memcached_replace(memcached_st *ptr, const char *key, size_t key_length,
                                     const char *value, size_t value_length, time_t expiration,
                                     uint32_t flags);
LIBMEMCACHED_API
memcached_return_t memcached_append(memcached_st *ptr, const char *key, size_t key_length,
                                    const char *value, size_t value_length, time_t expiration,
                                    uint32_t flags);
LIBMEMCACHED_API
memcached_return_t memcached_prepend(memcached_st *ptr, const char *key, size_t key_length,
                                     const char *value, size_t value_length, time_t expiration,
                                     uint32_t flags);
LIBMEMCACHED_API
memcached_return_t memcached_cas(memcached_st *ptr, const char *key, size_t key_length,
                                 const char *value, size_t value_length, time_t expiration,
                                 uint32_t flags, uint64_t cas);

LIBMEMCACHED_API
memcached_return_t memcached_set_by_key(memcached_st *ptr, const char *group_key,
                                        size_t group_key_length, const char *key, size_t key_length,
                                        const char *value, size_t value_length, time_t expiration,
                                        uint32_t flags);

LIBMEMCACHED_API
memcached_return_t memcached_add_by_key(memcached_st *ptr, const char *group_key,
                                        size_t group_key_length, const char *key, size_t key_length,
                                        const char *value, size_t value_length, time_t expiration,
                                        uint32_t flags);

LIBMEMCACHED_API
memcached_return_t memcached_replace_by_key(memcached_st *ptr, const char *group_key,
                                            size_t group_key_length, const char *key,
                                            size_t key_length, const char *value,
                                            size_t value_length, time_t expiration, uint32_t flags);

LIBMEMCACHED_API
memcached_return_t memcached_prepend_by_key(memcached_st *ptr, const char *group_key,
                                            size_t group_key_length, const char *key,
                                            size_t key_length, const char *value,
                                            size_t value_length, time_t expiration, uint32_t flags);

LIBMEMCACHED_API
memcached_return_t memcached_append_by_key(memcached_st *ptr, const char *group_key,
                                           size_t group_key_length, const char *key,
                                           size_t key_length, const char *value,
                                           size_t value_length, time_t expiration, uint32_t flags);

LIBMEMCACHED_API
memcached_return_t memcached_cas_by_key(memcached_st *ptr, const char *group_key,
                                        size_t group_key_length, const char *key, size_t key_length,
                                        const char *value, size_t value_length, time_t expiration,
                                        uint32_t flags, uint64_t cas);

#ifdef __cplusplus
}
#endif
