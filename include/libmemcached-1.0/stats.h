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

#include "libmemcached-1.0/struct/stat.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
void memcached_stat_free(const memcached_st *, memcached_stat_st *);

LIBMEMCACHED_API
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_stat_servername(memcached_stat_st *memc_stat, char *args,
                                             const char *hostname, in_port_t port);

LIBMEMCACHED_API
char *memcached_stat_get_value(const memcached_st *ptr, memcached_stat_st *memc_stat,
                               const char *key, memcached_return_t *error);

LIBMEMCACHED_API
char **memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *memc_stat,
                               memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_stat_execute(memcached_st *memc, const char *args,
                                          memcached_stat_fn func, void *context);

#ifdef __cplusplus
} // extern "C"
#endif
