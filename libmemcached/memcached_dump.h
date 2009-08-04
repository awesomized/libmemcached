/*
 * Summary: Dump function for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_DUMP_H
#define LIBMEMCACHED_MEMCACHED_DUMP_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_dump(memcached_st *ptr, memcached_dump_func *function, void *context, uint32_t number_of_callbacks);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_DUMP_H */
