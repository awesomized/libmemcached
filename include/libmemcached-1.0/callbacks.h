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

typedef memcached_return_t (*memcached_execute_fn)(const memcached_st *ptr,
                                                   memcached_result_st *result, void *context);
typedef memcached_return_t (*memcached_server_fn)(const memcached_st *ptr,
                                                  const memcached_instance_st *server,
                                                  void *context);
typedef memcached_return_t (*memcached_stat_fn)(const memcached_instance_st *server,
                                                const char *key, size_t key_length,
                                                const char *value, size_t value_length,
                                                void *context);

#ifdef __cplusplus
}
#endif
