/*
 * Summary: Typpes for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Brian Aker
 */

#ifndef LIBMEMCACHED_MEMCACHED_TYPES_H
#define LIBMEMCACHED_MEMCACHED_TYPES_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

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
typedef memcached_return (*memcached_clone_func)(memcached_st *parent, memcached_st *clone);
typedef memcached_return (*memcached_cleanup_func)(memcached_st *ptr);
typedef void (*memcached_free_function)(memcached_st *ptr, void *mem);
typedef void *(*memcached_malloc_function)(memcached_st *ptr, const size_t size);
typedef void *(*memcached_realloc_function)(memcached_st *ptr, void *mem, const size_t size);
typedef void *(*memcached_calloc_function)(memcached_st *ptr, size_t nelem, const size_t elsize);
typedef memcached_return (*memcached_execute_function)(memcached_st *ptr, memcached_result_st *result, void *context);
typedef memcached_return (*memcached_server_function)(memcached_st *ptr, memcached_server_st *server, void *context);
typedef memcached_return (*memcached_trigger_key)(memcached_st *ptr,  
                                                  const char *key, size_t key_length, 
                                                  memcached_result_st *result);
typedef memcached_return (*memcached_trigger_delete_key)(memcached_st *ptr,  
                                                         const char *key, size_t key_length);

typedef memcached_return (*memcached_dump_func)(memcached_st *ptr,  
                                                const char *key, size_t key_length, void *context);

struct memcached_string_st {
  memcached_st *root;
  char *end;
  char *string;
  size_t current_size;
  size_t block_size;
  bool is_allocated;
};

struct memcached_result_st {
  uint32_t flags;
  bool is_allocated;
  time_t expiration;
  memcached_st *root;
  size_t key_length;
  uint64_t cas;
  memcached_string_st value;
  char key[MEMCACHED_MAX_KEY];
  /* Add result callback function */
};

struct memcached_st {
  uint8_t purging;
  bool is_allocated;
  uint8_t distribution;
  uint8_t hash;
  uint32_t continuum_points_counter;
  memcached_server_st *hosts;
  int32_t snd_timeout;
  int32_t rcv_timeout;
  uint32_t server_failure_limit;
  uint32_t io_msg_watermark;
  uint32_t io_bytes_watermark;
  uint32_t io_key_prefetch;
  uint32_t number_of_hosts;
  uint32_t cursor_server;
  int cached_errno;
  uint32_t flags;
  int32_t poll_timeout;
  int32_t connect_timeout;
  int32_t retry_timeout;
  uint32_t continuum_count;
  int send_size;
  int recv_size;
  void *user_data;
  time_t next_distribution_rebuild;
  size_t prefix_key_length;
  memcached_hash hash_continuum;
  memcached_result_st result;
  memcached_continuum_item_st *continuum;
  memcached_clone_func on_clone;
  memcached_cleanup_func on_cleanup;
  memcached_free_function call_free;
  memcached_malloc_function call_malloc;
  memcached_realloc_function call_realloc;
  memcached_calloc_function call_calloc;
  memcached_trigger_key get_key_failure;
  memcached_trigger_delete_key delete_trigger;
  char prefix_key[MEMCACHED_PREFIX_KEY_MAX_SIZE];
  uint32_t number_of_replicas;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_TYPES_H */
