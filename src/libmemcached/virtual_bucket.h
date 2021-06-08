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

memcached_return_t memcached_virtual_bucket_create(memcached_st *self, const uint32_t *host_map,
                                                   const uint32_t *forward_map,
                                                   const uint32_t buckets, const uint32_t replicas);

uint32_t memcached_virtual_bucket_get(const memcached_st *self, uint32_t digest);

void memcached_virtual_bucket_free(memcached_st *self);

#ifdef __cplusplus
}
#endif
