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

#ifndef _WIN32
#  include <netdb.h>
#endif

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
pid_t libmemcached_util_getpid(const char *hostname, in_port_t port, memcached_return_t *ret);

LIBMEMCACHED_API
pid_t libmemcached_util_getpid2(const char *hostname, in_port_t port, const char *username,
                                const char *password, memcached_return_t *ret);

#ifdef __cplusplus
}
#endif
