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
memcached_return_t memcached_behavior_set(memcached_st *ptr, const memcached_behavior_t flag,
                                          uint64_t data);

LIBMEMCACHED_API
uint64_t memcached_behavior_get(memcached_st *ptr, const memcached_behavior_t flag);

LIBMEMCACHED_API
memcached_return_t memcached_behavior_set_distribution(memcached_st *ptr,
                                                       memcached_server_distribution_t type);

LIBMEMCACHED_API
memcached_server_distribution_t memcached_behavior_get_distribution(memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_behavior_set_key_hash(memcached_st *ptr, memcached_hash_t type);

LIBMEMCACHED_API
memcached_hash_t memcached_behavior_get_key_hash(memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_behavior_set_distribution_hash(memcached_st *ptr,
                                                            memcached_hash_t type);

LIBMEMCACHED_API
memcached_hash_t memcached_behavior_get_distribution_hash(memcached_st *ptr);

LIBMEMCACHED_API
const char *libmemcached_string_behavior(const memcached_behavior_t flag);

LIBMEMCACHED_API
const char *libmemcached_string_distribution(const memcached_server_distribution_t flag);

LIBMEMCACHED_API
memcached_return_t memcached_bucket_set(memcached_st *self, const uint32_t *host_map,
                                        const uint32_t *forward_map, const uint32_t buckets,
                                        const uint32_t replicas);

#ifdef __cplusplus
}
#endif
