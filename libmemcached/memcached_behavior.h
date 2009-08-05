/*
 * Summary: Behavior functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_BEHAVIOR_H
#define LIBMEMCACHED_MEMCACHED_BEHAVIOR_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_behavior_set(memcached_st *ptr, memcached_behavior flag, uint64_t data);
LIBMEMCACHED_API
uint64_t memcached_behavior_get(memcached_st *ptr, memcached_behavior flag);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_BEHAVIOR_H */
