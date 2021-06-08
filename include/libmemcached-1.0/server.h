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

#include "libmemcached-1.0/struct/server.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return_t memcached_server_cursor(const memcached_st *ptr,
                                           const memcached_server_fn *callback, void *context,
                                           uint32_t number_of_callbacks);

LIBMEMCACHED_API
const memcached_instance_st *memcached_server_by_key(memcached_st *ptr, const char *key,
                                                     size_t key_length, memcached_return_t *error);

LIBMEMCACHED_API
void memcached_server_error_reset(memcached_server_st *ptr);

LIBMEMCACHED_API
void memcached_server_free(memcached_server_st *ptr);

LIBMEMCACHED_API
const memcached_instance_st *memcached_server_get_last_disconnect(const memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_server_add_udp(memcached_st *ptr, const char *hostname,
                                            in_port_t port);
LIBMEMCACHED_API
memcached_return_t memcached_server_add_unix_socket(memcached_st *ptr, const char *filename);
LIBMEMCACHED_API
memcached_return_t memcached_server_add(memcached_st *ptr, const char *hostname, in_port_t port);

LIBMEMCACHED_API
memcached_return_t memcached_server_add_udp_with_weight(memcached_st *ptr, const char *hostname,
                                                        in_port_t port, uint32_t weight);
LIBMEMCACHED_API
memcached_return_t memcached_server_add_unix_socket_with_weight(memcached_st *ptr,
                                                                const char *filename,
                                                                uint32_t weight);
LIBMEMCACHED_API
memcached_return_t memcached_server_add_with_weight(memcached_st *ptr, const char *hostname,
                                                    in_port_t port, uint32_t weight);

/**
  Operations on Single Servers.
*/
LIBMEMCACHED_API
uint32_t memcached_server_response_count(const memcached_instance_st *self);

LIBMEMCACHED_API
const char *memcached_server_name(const memcached_instance_st *self);

LIBMEMCACHED_API
in_port_t memcached_server_port(const memcached_instance_st *self);

LIBMEMCACHED_API
in_port_t memcached_server_srcport(const memcached_instance_st *self);

LIBMEMCACHED_API
void memcached_instance_next_retry(const memcached_instance_st *self, const time_t absolute_time);

LIBMEMCACHED_API
const char *memcached_server_type(const memcached_instance_st *ptr);

LIBMEMCACHED_API
uint8_t memcached_server_major_version(const memcached_instance_st *ptr);

LIBMEMCACHED_API
uint8_t memcached_server_minor_version(const memcached_instance_st *ptr);

LIBMEMCACHED_API
uint8_t memcached_server_micro_version(const memcached_instance_st *ptr);

#ifdef __cplusplus
} // extern "C"
#endif
