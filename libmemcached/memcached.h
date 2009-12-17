/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: interface for memcached server
 * Description: main include file for libmemcached
 *
 */

#ifndef __MEMCACHED_H__
#define __MEMCACHED_H__

#include <stdlib.h>
#include <inttypes.h>

#if !defined(__cplusplus)
# include <stdbool.h>
#endif

#include <sys/types.h>
#include <netinet/in.h>

#include <libmemcached/visibility.h>
#include <libmemcached/memcached_configure.h>
#include <libmemcached/constants.h>
#include <libmemcached/types.h>
#include <libmemcached/string.h>
// Everything above this line must be in the order specified.
#include <libmemcached/behavior.h>
#include <libmemcached/callback.h>
#include <libmemcached/dump.h>
#include <libmemcached/get.h>
#include <libmemcached/result.h>
#include <libmemcached/server.h>
#include <libmemcached/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MEMCACHED_VERSION_STRING_LENGTH 24

struct memcached_analysis_st {
  uint32_t average_item_size;
  uint32_t longest_uptime;
  uint32_t least_free_server;
  uint32_t most_consumed_server;
  uint32_t oldest_server;
  double pool_hit_ratio;
  uint64_t most_used_bytes;
  uint64_t least_remaining_bytes;
};

struct memcached_stat_st {
  uint32_t connection_structures;
  uint32_t curr_connections;
  uint32_t curr_items;
  uint32_t pid;
  uint32_t pointer_size;
  uint32_t rusage_system_microseconds;
  uint32_t rusage_system_seconds;
  uint32_t rusage_user_microseconds;
  uint32_t rusage_user_seconds;
  uint32_t threads;
  uint32_t time;
  uint32_t total_connections;
  uint32_t total_items;
  uint32_t uptime;
  uint64_t bytes;
  uint64_t bytes_read;
  uint64_t bytes_written;
  uint64_t cmd_get;
  uint64_t cmd_set;
  uint64_t evictions;
  uint64_t get_hits;
  uint64_t get_misses;
  uint64_t limit_maxbytes;
  char version[MEMCACHED_VERSION_STRING_LENGTH];
};

struct memcached_st {
  struct {
    bool is_allocated:1;
    bool is_initialized:1;
    bool is_purging:1;
  } options;
  memcached_server_distribution_t distribution;
  memcached_hash_t hash;
  uint32_t continuum_points_counter;
  memcached_server_st *hosts;
  memcached_server_st *last_disconnected_server;
  int32_t snd_timeout;
  int32_t rcv_timeout;
  uint32_t server_failure_limit;
  uint32_t io_msg_watermark;
  uint32_t io_bytes_watermark;
  uint32_t io_key_prefetch;
  uint32_t number_of_hosts;
  uint32_t cursor_server;
  int cached_errno;
  struct {
    bool auto_eject_hosts:1;
    bool binary_protocol:1;
    bool buffer_requests:1;
    bool hash_with_prefix_key:1;
    bool ketama_weighted:1;
    bool no_block:1;
    bool no_reply:1;
    bool randomize_replica_read:1;
    bool reuse_memory:1;
    bool support_cas:1;
    bool tcp_nodelay:1;
    bool use_cache_lookups:1;
    bool use_sort_hosts:1;
    bool use_udp:1;
    bool verify_key:1;
  } flags;
  int32_t poll_timeout;
  int32_t connect_timeout;
  int32_t retry_timeout;
  uint32_t continuum_count;
  int send_size;
  int recv_size;
  void *user_data;
  time_t next_distribution_rebuild;
  size_t prefix_key_length;
  uint32_t number_of_replicas;
  memcached_hash_t distribution_hash;
  memcached_result_st result;
  memcached_continuum_item_st *continuum;
  memcached_clone_fn on_clone;
  memcached_cleanup_fn on_cleanup;
  memcached_free_fn call_free;
  memcached_malloc_fn call_malloc;
  memcached_realloc_fn call_realloc;
  memcached_calloc_fn call_calloc;
  memcached_trigger_key_fn get_key_failure;
  memcached_trigger_delete_key_fn delete_trigger;
  memcached_callback_st *callbacks;
  char prefix_key[MEMCACHED_PREFIX_KEY_MAX_SIZE];
};

LIBMEMCACHED_API
memcached_return_t memcached_version(memcached_st *ptr);

/* Public API */

LIBMEMCACHED_API
const char * memcached_lib_version(void);

LIBMEMCACHED_API
memcached_st *memcached_create(memcached_st *ptr);
LIBMEMCACHED_API
void memcached_free(memcached_st *ptr);
LIBMEMCACHED_API
memcached_st *memcached_clone(memcached_st *clone, memcached_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_delete(memcached_st *ptr, const char *key, size_t key_length,
                                  time_t expiration);
LIBMEMCACHED_API
memcached_return_t memcached_increment(memcached_st *ptr,
                                     const char *key, size_t key_length,
                                     uint32_t offset,
                                     uint64_t *value);
LIBMEMCACHED_API
memcached_return_t memcached_decrement(memcached_st *ptr,
                                     const char *key, size_t key_length,
                                     uint32_t offset,
                                     uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_increment_by_key(memcached_st *ptr, 
                                            const char *master_key, size_t master_key_length,
                                            const char *key, size_t key_length,
                                            uint64_t offset,
                                            uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_decrement_by_key(memcached_st *ptr, 
                                            const char *master_key, size_t master_key_length,
                                            const char *key, size_t key_length,
                                            uint64_t offset,
                                            uint64_t *value);

LIBMEMCACHED_API
memcached_return_t memcached_increment_with_initial(memcached_st *ptr,
                                                  const char *key,
                                                  size_t key_length,
                                                  uint64_t offset,
                                                  uint64_t initial,
                                                  time_t expiration,
                                                  uint64_t *value);
LIBMEMCACHED_API
memcached_return_t memcached_decrement_with_initial(memcached_st *ptr,
                                                  const char *key,
                                                  size_t key_length,
                                                  uint64_t offset,
                                                  uint64_t initial,
                                                  time_t expiration,
                                                  uint64_t *value);
LIBMEMCACHED_API
memcached_return_t memcached_increment_with_initial_by_key(memcached_st *ptr,
                                                         const char *master_key,
                                                         size_t master_key_length,
                                                         const char *key,
                                                         size_t key_length,
                                                         uint64_t offset,
                                                         uint64_t initial,
                                                         time_t expiration,
                                                         uint64_t *value);
LIBMEMCACHED_API
memcached_return_t memcached_decrement_with_initial_by_key(memcached_st *ptr,
                                                         const char *master_key,
                                                         size_t master_key_length,
                                                         const char *key,
                                                         size_t key_length,
                                                         uint64_t offset,
                                                         uint64_t initial,
                                                         time_t expiration,
                                                         uint64_t *value);
LIBMEMCACHED_API
void memcached_stat_free(memcached_st *, memcached_stat_st *);

LIBMEMCACHED_API
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_stat_servername(memcached_stat_st *memc_stat, char *args,
                                             const char *hostname, in_port_t port);

LIBMEMCACHED_API
memcached_return_t memcached_flush(memcached_st *ptr, time_t expiration);

LIBMEMCACHED_API
memcached_return_t memcached_verbosity(memcached_st *ptr, unsigned int verbosity);

LIBMEMCACHED_API
void memcached_quit(memcached_st *ptr);

LIBMEMCACHED_API
const char *memcached_strerror(memcached_st *ptr, memcached_return_t rc);

/* The two public hash bits */
LIBMEMCACHED_API
uint32_t memcached_generate_hash_value(const char *key, size_t key_length, memcached_hash_t hash_algorithm);

LIBMEMCACHED_API
uint32_t memcached_generate_hash(memcached_st *ptr, const char *key, size_t key_length);

LIBMEMCACHED_API
memcached_return_t memcached_flush_buffers(memcached_st *mem);

/* Server Public functions */

LIBMEMCACHED_API
memcached_return_t memcached_server_add_udp(memcached_st *ptr,
                                          const char *hostname,
                                          in_port_t port);
LIBMEMCACHED_API
memcached_return_t memcached_server_add_unix_socket(memcached_st *ptr,
                                                  const char *filename);
LIBMEMCACHED_API
memcached_return_t memcached_server_add(memcached_st *ptr, 
                                        const char *hostname, in_port_t port);

LIBMEMCACHED_API
memcached_return_t memcached_server_add_udp_with_weight(memcached_st *ptr,
                                                      const char *hostname,
                                                      in_port_t port,
                                                      uint32_t weight);
LIBMEMCACHED_API
memcached_return_t memcached_server_add_unix_socket_with_weight(memcached_st *ptr,
                                                              const char *filename,
                                                              uint32_t weight);
LIBMEMCACHED_API
memcached_return_t memcached_server_add_with_weight(memcached_st *ptr, const char *hostname,
                                                  in_port_t port,
                                                  uint32_t weight);
LIBMEMCACHED_API
void memcached_server_list_free(memcached_server_st *ptr);

LIBMEMCACHED_API
memcached_return_t memcached_server_push(memcached_st *ptr, memcached_server_st *list);

LIBMEMCACHED_API
memcached_server_st *memcached_server_list_append(memcached_server_st *ptr,
                                                  const char *hostname,
                                                  in_port_t port,
                                                  memcached_return_t *error);
LIBMEMCACHED_API
memcached_server_st *memcached_server_list_append_with_weight(memcached_server_st *ptr,
                                                              const char *hostname,
                                                              in_port_t port,
                                                              uint32_t weight,
                                                              memcached_return_t *error);
LIBMEMCACHED_API
unsigned int memcached_server_list_count(memcached_server_st *ptr);

LIBMEMCACHED_API
memcached_server_st *memcached_servers_parse(const char *server_strings);

LIBMEMCACHED_API
char *memcached_stat_get_value(memcached_st *ptr, memcached_stat_st *memc_stat,
                               const char *key, memcached_return_t *error);
LIBMEMCACHED_API
char ** memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *memc_stat,
                                memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_delete_by_key(memcached_st *ptr,
                                           const char *master_key, size_t master_key_length,
                                           const char *key, size_t key_length,
                                           time_t expiration);

LIBMEMCACHED_API
memcached_return_t memcached_fetch_execute(memcached_st *ptr,
                                           memcached_execute_fn *callback,
                                           void *context,
                                           unsigned int number_of_callbacks);

LIBMEMCACHED_API
memcached_return_t memcached_set_memory_allocators(memcached_st *ptr,
                                                   memcached_malloc_fn mem_malloc,
                                                   memcached_free_fn mem_free,
                                                   memcached_realloc_fn mem_realloc,
                                                   memcached_calloc_fn mem_calloc);

LIBMEMCACHED_API
void memcached_get_memory_allocators(memcached_st *ptr,
                                     memcached_malloc_fn *mem_malloc,
                                     memcached_free_fn *mem_free,
                                     memcached_realloc_fn *mem_realloc,
                                     memcached_calloc_fn *mem_calloc);

LIBMEMCACHED_API
void *memcached_get_user_data(memcached_st *ptr);
LIBMEMCACHED_API
void *memcached_set_user_data(memcached_st *ptr, void *data);

LIBMEMCACHED_LOCAL
memcached_return_t run_distribution(memcached_st *ptr);

#define memcached_is_allocated(__object) ((__object)->options.is_allocated)
#define memcached_is_initialized(__object) ((__object)->options.is_initialized)

#ifdef __cplusplus
}
#endif


#endif /* __MEMCACHED_H__ */
