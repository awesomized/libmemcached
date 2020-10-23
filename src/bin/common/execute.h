/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include <stdio.h>

#include "libmemcached-1.0/memcached.h"
#include "generator.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned int execute_set(memcached_st *memc, pairs_st *pairs, unsigned int number_of);
unsigned int execute_get(memcached_st *memc, pairs_st *pairs, unsigned int number_of);
unsigned int execute_mget(memcached_st *memc, const char *const *keys, size_t *key_length,
                          unsigned int number_of);

#ifdef __cplusplus
} // extern "C"
#endif
