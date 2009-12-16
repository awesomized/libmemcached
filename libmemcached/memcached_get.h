/*
 * Summary: Get functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Brian Aker
 */

#ifndef LIBMEMCACHED_MEMCACHED_GET_H
#define LIBMEMCACHED_MEMCACHED_GET_H

#ifdef __cplusplus
extern "C" {
#endif

/* Public defines */
LIBMEMCACHED_API
char *memcached_get(memcached_st *ptr, 
                    const char *key, size_t key_length,
                    size_t *value_length, 
                    uint32_t *flags,
                    memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_mget(memcached_st *ptr, 
                                  const char * const *keys, 
                                  const size_t *key_length, 
                                  size_t number_of_keys);

LIBMEMCACHED_API
char *memcached_get_by_key(memcached_st *ptr, 
                           const char *master_key, size_t master_key_length, 
                           const char *key, size_t key_length, 
                           size_t *value_length, 
                           uint32_t *flags,
                           memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_mget_by_key(memcached_st *ptr, 
                                         const char *master_key, size_t 
                                         master_key_length,
                                         const char * const *keys, 
                                         const size_t *key_length, 
                                         size_t number_of_keys);

LIBMEMCACHED_API
char *memcached_fetch(memcached_st *ptr, 
                      char *key, size_t *key_length, 
                      size_t *value_length, uint32_t *flags, 
                      memcached_return_t *error);

LIBMEMCACHED_API
memcached_result_st *memcached_fetch_result(memcached_st *ptr, 
                                            memcached_result_st *result,
                                            memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_mget_execute(memcached_st *ptr,
                                          const char * const *keys,
                                          const size_t *key_length,
                                          size_t number_of_keys,
                                          memcached_execute_fn *callback,
                                          void *context,
                                          unsigned int number_of_callbacks);

LIBMEMCACHED_API
memcached_return_t memcached_mget_execute_by_key(memcached_st *ptr,
                                                 const char *master_key,
                                                 size_t master_key_length,
                                                 const char * const *keys,
                                                 const size_t *key_length,
                                                 size_t number_of_keys,
                                                 memcached_execute_fn *callback,
                                                 void *context,
                                                 unsigned int number_of_callbacks);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_GET_H */
