/*
 * Summary: Flush functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_FLUSH_H
#define LIBMEMCACHED_MEMCACHED_FLUSH_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_flush(memcached_st *ptr, time_t expiration);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_FLUSH_H */
