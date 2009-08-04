/*
 * Summary: interface for memcached server
 * Description: main include file for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Brian Aker
 */

#ifndef LIBMEMCACHED_MEMCACHED_H
#define LIBMEMCACHED_MEMCACHED_H

#include <stdlib.h>
#include <inttypes.h>
#if !defined(__cplusplus)
# include <stdbool.h>
#endif
#include <sys/types.h>
#include <netinet/in.h>

#define MEMCACHED_VERSION_STRING_LENGTH 24
#define LIBMEMCACHED_VERSION_STRING "0.31"

#include <libmemcached/visibility.h>
#include <libmemcached/memcached_configure.h>
#include <libmemcached/memcached_constants.h>
#include <libmemcached/memcached_types.h>
#include <libmemcached/memcached_get.h>
#include <libmemcached/memcached_server.h>
#include <libmemcached/memcached_string.h>
#include <libmemcached/memcached_result.h>
#include <libmemcached/memcached_storage.h>
#include <libmemcached/memcached_stats.h>
#include <libmemcached/memcached_arithmetic.h>
#include <libmemcached/memcached_allocators.h>
#include <libmemcached/memcached_behavior.h>
#include <libmemcached/memcached_callback.h>
#include <libmemcached/memcached_dump.h>
#include <libmemcached/memcached_delete.h>
#include <libmemcached/memcached_flush.h>
#include <libmemcached/memcached_flush_buffers.h>
#include <libmemcached/memcached_hash.h>
#include <libmemcached/memcached_st.h>

#ifdef __cplusplus
#include <libmemcached/memcached.hpp>
#endif

#endif /* LIBMEMCACHED_MEMCACHED_H */
