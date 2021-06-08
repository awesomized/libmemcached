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

typedef memcached_return_t (*memcached_clone_fn)(memcached_st *destination,
                                                 const memcached_st *source);
typedef memcached_return_t (*memcached_cleanup_fn)(const memcached_st *ptr);

/**
  Trigger functions.
*/
typedef memcached_return_t (*memcached_trigger_key_fn)(const memcached_st *ptr, const char *key,
                                                       size_t key_length,
                                                       memcached_result_st *result);
typedef memcached_return_t (*memcached_trigger_delete_key_fn)(const memcached_st *ptr,
                                                              const char *key, size_t key_length);

typedef memcached_return_t (*memcached_dump_fn)(const memcached_st *ptr, const char *key,
                                                size_t key_length, void *context);

#ifdef __cplusplus
}
#endif
