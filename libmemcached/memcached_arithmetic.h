/*
 * Summary: Arithmetic functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_ARITHMETIC_H
#define LIBMEMCACHED_MEMCACHED_ARITHMETIC_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_increment(memcached_st *ptr, 
                                     const char *key, size_t key_length,
                                     uint32_t offset,
                                     uint64_t *value);
LIBMEMCACHED_API
memcached_return memcached_decrement(memcached_st *ptr, 
                                     const char *key, size_t key_length,
                                     uint32_t offset,
                                     uint64_t *value);
LIBMEMCACHED_API
memcached_return memcached_increment_with_initial(memcached_st *ptr,
                                                  const char *key,
                                                  size_t key_length,
                                                  uint64_t offset,
                                                  uint64_t initial,
                                                  time_t expiration,
                                                  uint64_t *value);
LIBMEMCACHED_API
memcached_return memcached_decrement_with_initial(memcached_st *ptr,
                                                  const char *key,
                                                  size_t key_length,
                                                  uint64_t offset,
                                                  uint64_t initial,
                                                  time_t expiration,
                                                  uint64_t *value);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_ARITHMETIC_H */
