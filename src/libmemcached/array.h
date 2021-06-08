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

memcached_array_st *memcached_array_clone(Memcached *memc, const memcached_array_st *original);

memcached_array_st *memcached_strcpy(Memcached *memc, const char *str, size_t str_length);

void memcached_array_free(memcached_array_st *array);

size_t memcached_array_size(memcached_array_st *array);

const char *memcached_array_string(memcached_array_st *array);

memcached_string_t memcached_array_to_string(memcached_array_st *array);

bool memcached_array_is_null(memcached_array_st *array);

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#  define memcached_print_array(X) \
    static_cast<int>(memcached_array_size(X)), memcached_array_string(X)
#  define memcached_param_array(X) memcached_array_string(X), memcached_array_size(X)
#else
#  define memcached_print_array(X) (int) memcached_array_size((X)), memcached_array_string((X))
#  define memcached_param_array(X) memcached_array_string(X), memcached_array_size(X)
#endif
