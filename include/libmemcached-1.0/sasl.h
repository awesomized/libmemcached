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

#if defined(LIBMEMCACHED_WITH_SASL_SUPPORT) && LIBMEMCACHED_WITH_SASL_SUPPORT
#  include <sasl/sasl.h>
#else
#  define sasl_callback_t void
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
void memcached_set_sasl_callbacks(memcached_st *ptr, const sasl_callback_t *callbacks);

LIBMEMCACHED_API
memcached_return_t memcached_set_sasl_auth_data(memcached_st *ptr, const char *username,
                                                const char *password);

LIBMEMCACHED_API
memcached_return_t memcached_destroy_sasl_auth_data(memcached_st *ptr);

LIBMEMCACHED_API
sasl_callback_t *memcached_get_sasl_callbacks(memcached_st *ptr);

#ifdef __cplusplus
}
#endif

#include "libmemcached-1.0/struct/sasl.h"
