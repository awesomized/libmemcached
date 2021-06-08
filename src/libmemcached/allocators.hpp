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

void _libmemcached_free(const memcached_st *ptr, void *mem, void *context);

void *_libmemcached_malloc(const memcached_st *ptr, const size_t size, void *context);

void *_libmemcached_realloc(const memcached_st *ptr, void *mem, const size_t size, void *context);

void *_libmemcached_calloc(const memcached_st *ptr, size_t nelem, size_t size, void *context);

struct memcached_allocator_t memcached_allocators_return_default(void);
