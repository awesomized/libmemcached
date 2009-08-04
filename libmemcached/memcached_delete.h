/*
 * Summary: Delete functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_DELETE_H
#define LIBMEMCACHED_MEMCACHED_DELETE_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_delete(memcached_st *ptr, const char *key, size_t key_length,
                                  time_t expiration);

LIBMEMCACHED_API
memcached_return memcached_delete_by_key(memcached_st *ptr, 
                                         const char *master_key, size_t master_key_length,
                                         const char *key, size_t key_length,
                                         time_t expiration);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_DELETE_H */
