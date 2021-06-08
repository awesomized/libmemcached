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

#include "libmemcached-1.0/configure.h"
#include "libmemcached-1.0/visibility.h"
#include "libmemcached-1.0/platform.h"

#include "libmemcached-1.0/limits.h"
#include "libmemcached-1.0/defaults.h"

#include "libmemcached-1.0/types/behavior.h"
#include "libmemcached-1.0/types/callback.h"
#include "libmemcached-1.0/types/connection.h"
#include "libmemcached-1.0/types/hash.h"
#include "libmemcached-1.0/types/return.h"
#include "libmemcached-1.0/types/server_distribution.h"

#include "libmemcached-1.0/return.h"

#include "libmemcached-1.0/types.h"
#include "libmemcached-1.0/callbacks.h"
#include "libmemcached-1.0/alloc.h"
#include "libmemcached-1.0/triggers.h"

#include "libhashkit-1.0/hashkit.h"

#include "libmemcached-1.0/struct/callback.h"
#include "libmemcached-1.0/struct/string.h"
#include "libmemcached-1.0/struct/result.h"
#include "libmemcached-1.0/struct/allocator.h"
#include "libmemcached-1.0/struct/sasl.h"
#include "libmemcached-1.0/struct/memcached.h"
#include "libmemcached-1.0/struct/server.h"
#include "libmemcached-1.0/struct/stat.h"

#include "libmemcached-1.0/basic_string.h"
#include "libmemcached-1.0/error.h"
#include "libmemcached-1.0/stats.h"

// Everything above this line must be in the order specified.
#include "libmemcached-1.0/allocators.h"
#include "libmemcached-1.0/analyze.h"
#include "libmemcached-1.0/auto.h"
#include "libmemcached-1.0/behavior.h"
#include "libmemcached-1.0/callback.h"
#include "libmemcached-1.0/delete.h"
#include "libmemcached-1.0/dump.h"
#include "libmemcached-1.0/encoding_key.h"
#include "libmemcached-1.0/exist.h"
#include "libmemcached-1.0/fetch.h"
#include "libmemcached-1.0/flush.h"
#include "libmemcached-1.0/flush_buffers.h"
#include "libmemcached-1.0/get.h"
#include "libmemcached-1.0/hash.h"
#include "libmemcached-1.0/options.h"
#include "libmemcached-1.0/parse.h"
#include "libmemcached-1.0/quit.h"
#include "libmemcached-1.0/result.h"
#include "libmemcached-1.0/server.h"
#include "libmemcached-1.0/server_list.h"
#include "libmemcached-1.0/storage.h"
#include "libmemcached-1.0/strerror.h"
#include "libmemcached-1.0/touch.h"
#include "libmemcached-1.0/verbosity.h"
#include "libmemcached-1.0/version.h"
#include "libmemcached-1.0/sasl.h"

#include "libmemcached-1.0/deprecated_types.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
void memcached_servers_reset(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached_create(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached(const char *string, size_t string_length);

LIBMEMCACHED_API
void memcached_free(memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_reset(memcached_st *ptr);

LIBMEMCACHED_API
void memcached_reset_last_disconnected_server(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached_clone(memcached_st *clone, const memcached_st *ptr);

LIBMEMCACHED_API
void *memcached_get_user_data(const memcached_st *ptr);

LIBMEMCACHED_API
void *memcached_set_user_data(memcached_st *ptr, void *data);

LIBMEMCACHED_API
memcached_return_t memcached_push(memcached_st *destination, const memcached_st *source);

LIBMEMCACHED_API
const memcached_instance_st *memcached_server_instance_by_position(const memcached_st *ptr,
                                                                   uint32_t server_key);

LIBMEMCACHED_API
uint32_t memcached_server_count(const memcached_st *);

LIBMEMCACHED_API
uint64_t memcached_query_id(const memcached_st *);

#ifdef __cplusplus
} // extern "C"
#endif
