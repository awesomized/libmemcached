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

#include "mem_config.h"

#ifdef __cplusplus
#  include <cstddef>
#  include <cstdio>
#  include <cstdlib>
#  include <cstring>
#  include <ctime>
#  include <cctype>
#  include <cerrno>
#  include <climits>
#  include <ciso646>
#else
#  include <stddef.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <time.h>
#  include <errno.h>
#  include <limits.h>
#  include <iso646.h>
#endif

#include "p9y/socket.hpp"

#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif

#include <fcntl.h>

#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif

#ifdef HAVE_DLFCN_H
#  include <dlfcn.h>
#endif

#include "libmemcached-1.0/memcached.h"
#include "libmemcached/watchpoint.h"
#include "libmemcached/is.h"
typedef struct memcached_st Memcached;

#ifdef __cplusplus
memcached_instance_st *memcached_instance_fetch(memcached_st *ptr, uint32_t server_key);
#endif

/* These are private not to be installed headers */
#include "libmemcached/error.hpp"
#include "libmemcached/memory.h"
#include "libmemcached/io.h"
#ifdef __cplusplus
#  include "libmemcached/string.hpp"
#  include "libmemcachedprotocol-0.0/binary.h"
#  include "libmemcached/io.hpp"
#  include "libmemcached/udp.hpp"
#  include "libmemcached/do.hpp"
#  include "libmemcached/connect.hpp"
#  include "libmemcached/allocators.hpp"
#  include "libmemcached/hash.hpp"
#  include "libmemcached/quit.hpp"
#  include "libmemcached/instance.hpp"
#  include "libmemcached/server_instance.h"
#  include "libmemcached/server.hpp"
#  include "libmemcached/flag.hpp"
#  include "libmemcached/behavior.hpp"
#  include "libmemcached/sasl.hpp"
#  include "libmemcached/server_list.hpp"
#endif

#include "libmemcached/internal.h"
#include "libmemcached/array.h"
#include "libmemcached/libmemcached_probes.h"
#include "libmemcached/byteorder.h"
#include "libmemcached/initialize_query.h"

#ifdef __cplusplus
#  include "libmemcached/response.h"
#  include "libmemcached/namespace.h"
#else
#  include "libmemcached/virtual_bucket.h"
#endif

#ifdef __cplusplus
#  include "libmemcached/backtrace.hpp"
#  include "libmemcached/assert.hpp"
#  include "libmemcached/server.hpp"
#  include "libmemcached/key.hpp"
#  include "libmemcached/result.h"
#  include "libmemcached/version.hpp"
#endif

#include "libmemcached/continuum.hpp"

#if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)

#  define likely(x)   if ((x))
#  define unlikely(x) if ((x))

#else

#  define likely(x)   if (__builtin_expect((x) != 0, 1))
#  define unlikely(x) if (__builtin_expect((x) != 0, 0))
#endif

#define MEMCACHED_BLOCK_SIZE           1024
#define MEMCACHED_DEFAULT_COMMAND_SIZE 350
#define SMALL_STRING_LEN               1024
#define HUGE_STRING_LEN                8196

#ifdef __cplusplus
extern "C" {
#endif

memcached_return_t run_distribution(memcached_st *ptr);

#ifdef __cplusplus
#  include "p9y/poll.hpp"
static inline void memcached_server_response_increment(memcached_instance_st *instance) {
  instance->events(POLLIN);
  instance->cursor_active_++;
}
#endif

#define memcached_server_response_decrement(A) do {     \
    WATCHPOINT_ASSERT((A)->cursor_active_ > 0);         \
    if ((A)->cursor_active_ > 0) {                      \
        (A)->cursor_active_--;                          \
    }                                                   \
} while (0)
#define memcached_server_response_reset(A)       (A)->cursor_active_ = 0

#define memcached_instance_response_increment(A) (A)->cursor_active_++
#define memcached_instance_response_decrement(A) do {   \
    WATCHPOINT_ASSERT((A)->cursor_active_ > 0);         \
    if ((A)->cursor_active_ > 0) {                      \
        (A)->cursor_active_--;                          \
    }                                                   \
} while (0)
#define memcached_instance_response_reset(A)     (A)->cursor_active_ = 0

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
bool memcached_purge(memcached_instance_st *);
memcached_instance_st *memcached_instance_by_position(const memcached_st *ptr, uint32_t server_key);
#endif
