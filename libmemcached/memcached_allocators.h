/*
 * Summary: Allocator functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_ALLOCATORS_H
#define LIBMEMCACHED_MEMCACHED_ALLOCATORS_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_set_memory_allocators(memcached_st *ptr,
                                                 memcached_malloc_function mem_malloc,
                                                 memcached_free_function mem_free,
                                                 memcached_realloc_function mem_realloc,
                                                 memcached_calloc_function mem_calloc);

LIBMEMCACHED_API
void memcached_get_memory_allocators(memcached_st *ptr,
                                     memcached_malloc_function *mem_malloc,
                                     memcached_free_function *mem_free,
                                     memcached_realloc_function *mem_realloc,
                                     memcached_calloc_function *mem_calloc);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_ALLOCATORS_H */
