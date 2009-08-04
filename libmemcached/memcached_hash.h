/*
 * Summary: Hash functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_HASH_H
#define LIBMEMCACHED_MEMCACHED_HASH_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
uint32_t memcached_generate_hash_value(const char *key, size_t key_length, memcached_hash hash_algorithm);
LIBMEMCACHED_API
uint32_t memcached_generate_hash(memcached_st *ptr, const char *key, size_t key_length);


#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_HASH_H */
