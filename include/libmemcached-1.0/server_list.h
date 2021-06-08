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

/* Server List Public functions */
LIBMEMCACHED_API
void memcached_server_list_free(memcached_server_list_st ptr);

LIBMEMCACHED_API
memcached_return_t memcached_server_push(memcached_st *ptr, const memcached_server_list_st list);

LIBMEMCACHED_API
memcached_server_list_st memcached_server_list_append(memcached_server_list_st ptr,
                                                      const char *hostname, in_port_t port,
                                                      memcached_return_t *error);
LIBMEMCACHED_API
memcached_server_list_st memcached_server_list_append_with_weight(memcached_server_list_st ptr,
                                                                  const char *hostname,
                                                                  in_port_t port, uint32_t weight,
                                                                  memcached_return_t *error);
LIBMEMCACHED_API
uint32_t memcached_server_list_count(const memcached_server_list_st ptr);

#ifdef __cplusplus
} // extern "C"
#endif
