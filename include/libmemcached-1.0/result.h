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

#include "libmemcached-1.0/struct/result.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Result Struct */
LIBMEMCACHED_API
void memcached_result_free(memcached_result_st *result);

LIBMEMCACHED_API
void memcached_result_reset(memcached_result_st *ptr);

LIBMEMCACHED_API
memcached_result_st *memcached_result_create(const memcached_st *ptr, memcached_result_st *result);

LIBMEMCACHED_API
const char *memcached_result_key_value(const memcached_result_st *self);

LIBMEMCACHED_API
size_t memcached_result_key_length(const memcached_result_st *self);

LIBMEMCACHED_API
const char *memcached_result_value(const memcached_result_st *self);

LIBMEMCACHED_API
char *memcached_result_take_value(memcached_result_st *self);

LIBMEMCACHED_API
size_t memcached_result_length(const memcached_result_st *self);

LIBMEMCACHED_API
uint32_t memcached_result_flags(const memcached_result_st *self);

LIBMEMCACHED_API
uint64_t memcached_result_cas(const memcached_result_st *self);

LIBMEMCACHED_API
memcached_return_t memcached_result_set_value(memcached_result_st *ptr, const char *value,
                                              size_t length);

LIBMEMCACHED_API
void memcached_result_set_flags(memcached_result_st *self, uint32_t flags);

LIBMEMCACHED_API
void memcached_result_set_expiration(memcached_result_st *self, time_t expiration);

#ifdef __cplusplus
} // extern "C"
#endif
