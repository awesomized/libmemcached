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

/**
  Memory allocation functions.
*/
typedef void (*memcached_free_fn)(const memcached_st *ptr, void *mem, void *context);
typedef void *(*memcached_malloc_fn)(const memcached_st *ptr, const size_t size, void *context);
typedef void *(*memcached_realloc_fn)(const memcached_st *ptr, void *mem, const size_t size,
                                      void *context);
typedef void *(*memcached_calloc_fn)(const memcached_st *ptr, size_t nelem, const size_t elsize,
                                     void *context);

#ifdef __cplusplus
}
#endif
