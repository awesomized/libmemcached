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

#include "libmemcached-1.0/memcached.h"

#ifdef __cplusplus
extern "C" {
#endif

struct memcached_pool_st;
typedef struct memcached_pool_st memcached_pool_st;

LIBMEMCACHED_API
memcached_pool_st *memcached_pool_create(memcached_st *mmc, uint32_t initial, uint32_t max);

LIBMEMCACHED_API
memcached_pool_st *memcached_pool(const char *option_string, size_t option_string_length);

LIBMEMCACHED_API
memcached_st *memcached_pool_destroy(memcached_pool_st *pool);

LIBMEMCACHED_API
memcached_st *memcached_pool_pop(memcached_pool_st *pool, bool block, memcached_return_t *rc);
LIBMEMCACHED_API
memcached_return_t memcached_pool_push(memcached_pool_st *pool, memcached_st *mmc);
LIBMEMCACHED_API
memcached_return_t memcached_pool_release(memcached_pool_st *pool, memcached_st *mmc);

LIBMEMCACHED_API
memcached_st *memcached_pool_fetch(memcached_pool_st *, struct timespec *relative_time,
                                   memcached_return_t *rc);

LIBMEMCACHED_API
memcached_return_t memcached_pool_behavior_set(memcached_pool_st *ptr, memcached_behavior_t flag,
                                               uint64_t data);
LIBMEMCACHED_API
memcached_return_t memcached_pool_behavior_get(memcached_pool_st *ptr, memcached_behavior_t flag,
                                               uint64_t *value);

#ifdef __cplusplus
} // extern "C"
#endif
