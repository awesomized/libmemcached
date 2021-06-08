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

LIBMEMCACHED_API
const char *memcached_error(const memcached_st *);

LIBMEMCACHED_API
const char *memcached_last_error_message(const memcached_st *);

LIBMEMCACHED_API
void memcached_error_print(const memcached_st *);

LIBMEMCACHED_API
memcached_return_t memcached_last_error(const memcached_st *);

LIBMEMCACHED_API
int memcached_last_error_errno(const memcached_st *);

LIBMEMCACHED_API
const char *memcached_server_error(const memcached_instance_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_server_error_return(const memcached_instance_st *ptr);

#ifdef __cplusplus
} // extern "C"
#endif
