/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Types for libmemcached
 *
 */

#ifndef LIBMEMCACHED_MEMCACHED_TYPES_H
#define LIBMEMCACHED_MEMCACHED_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memcached_st memcached_st;
typedef struct memcached_stat_st memcached_stat_st;
typedef struct memcached_analysis_st memcached_analysis_st;
typedef struct memcached_result_st memcached_result_st;
typedef struct memcached_string_st memcached_string_st;
typedef struct memcached_server_st memcached_server_st;
typedef struct memcached_continuum_item_st memcached_continuum_item_st;
typedef memcached_return_t (*memcached_clone_fn)(memcached_st *parent, memcached_st *clone);
typedef memcached_return_t (*memcached_cleanup_fn)(memcached_st *ptr);
typedef void (*memcached_free_fn)(memcached_st *ptr, void *mem);
typedef void *(*memcached_malloc_fn)(memcached_st *ptr, const size_t size);
typedef void *(*memcached_realloc_fn)(memcached_st *ptr, void *mem, const size_t size);
typedef void *(*memcached_calloc_fn)(memcached_st *ptr, size_t nelem, const size_t elsize);
typedef memcached_return_t (*memcached_execute_fn)(memcached_st *ptr, memcached_result_st *result, void *context);
typedef memcached_return_t (*memcached_server_fn)(memcached_st *ptr, memcached_server_st *server, void *context);
typedef memcached_return_t (*memcached_trigger_key_fn)(memcached_st *ptr,
                                                       const char *key, size_t key_length,
                                                       memcached_result_st *result);
typedef memcached_return_t (*memcached_trigger_delete_key_fn)(memcached_st *ptr,
                                                              const char *key, size_t key_length);

typedef memcached_return_t (*memcached_dump_fn)(memcached_st *ptr,
                                                const char *key,
                                                size_t key_length,
                                                void *context);

typedef struct {
  memcached_execute_fn *callback;
  void *context;
  uint32_t number_of_callback;
} memcached_callback_st;

 
/**
  @note The following definitions are just here for backwards compatibility.
*/
typedef memcached_return_t memcached_return;
typedef memcached_server_distribution_t memcached_server_distribution;
typedef memcached_behavior_t memcached_behavior;
typedef memcached_callback_t memcached_callback;
typedef memcached_hash_t memcached_hash;
typedef memcached_connection_t memcached_connection;
typedef memcached_clone_fn memcached_clone_func;
typedef memcached_cleanup_fn memcached_cleanup_func;
typedef memcached_free_fn memcached_free_function;
typedef memcached_malloc_fn memcached_malloc_function;
typedef memcached_realloc_fn memcached_realloc_function;
typedef memcached_calloc_fn memcached_calloc_function;
typedef memcached_execute_fn memcached_execute_function;
typedef memcached_server_fn memcached_server_function;
typedef memcached_trigger_key_fn memcached_trigger_key;
typedef memcached_trigger_delete_key_fn memcached_trigger_delete_key;
typedef memcached_dump_fn memcached_dump_func;
 

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_TYPES_H */
