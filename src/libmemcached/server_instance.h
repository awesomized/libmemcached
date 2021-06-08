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
typedef struct memcached_instance_st *memcached_server_write_instance_st;
#else
typedef void *memcached_server_write_instance_st;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef memcached_return_t (*memcached_server_execute_fn)(memcached_st *ptr,
                                                          memcached_server_write_instance_st server,
                                                          void *context);

memcached_return_t memcached_server_execute(memcached_st *ptr, memcached_server_execute_fn callback,
                                            void *context);

#ifdef __cplusplus
} // extern "C"
#endif
