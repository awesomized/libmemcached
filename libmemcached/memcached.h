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
#include <libmemcached/configure.h>
#include <libmemcached/constants.h>
#include <libmemcached/types.h>
#include <libmemcached/string.h>
#include <libmemcached/stats.h>
#include <libhashkit/hashkit.h>
// Everything above this line must be in the order specified.
#include <libmemcached/allocators.h>
#include <libmemcached/analyze.h>
#include <libmemcached/auto.h>
#include <libmemcached/behavior.h>
#include <libmemcached/callback.h>
#include <libmemcached/delete.h>
#include <libmemcached/dump.h>
#include <libmemcached/fetch.h>
#include <libmemcached/flush.h>
#include <libmemcached/flush_buffers.h>
#include <libmemcached/get.h>
#include <libmemcached/hash.h>
#include <libmemcached/parse.h>
#include <libmemcached/quit.h>
#include <libmemcached/result.h>
#include <libmemcached/server.h>
#include <libmemcached/storage.h>
#include <libmemcached/strerror.h>
#include <libmemcached/verbosity.h>
#include <libmemcached/version.h>

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
void memcached_servers_reset(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached_create(memcached_st *ptr);

LIBMEMCACHED_API
void memcached_free(memcached_st *ptr);

LIBMEMCACHED_API
memcached_st *memcached_clone(memcached_st *clone, const memcached_st *ptr);

LIBMEMCACHED_API
void *memcached_get_user_data(const memcached_st *ptr);

LIBMEMCACHED_API
void *memcached_set_user_data(memcached_st *ptr, void *data);


struct memcached_st {
  /**
    @note these are static and should not change without a call to behavior.
  */
  struct {
    bool is_purging:1;
    bool is_processing_input:1;
  } state;
  struct {
    // Everything below here is pretty static.
    bool auto_eject_hosts:1;
    bool binary_protocol:1;
    bool buffer_requests:1;
    bool cork:1;
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
  memcached_server_distribution_t distribution;
  hashkit_st hashkit;
  uint32_t continuum_points_counter; // Ketama
  uint32_t number_of_hosts;
  memcached_server_st *servers;
  memcached_server_st *last_disconnected_server;
  int32_t snd_timeout;
  int32_t rcv_timeout;
  uint32_t server_failure_limit;
  uint32_t io_msg_watermark;
  uint32_t io_bytes_watermark;
  uint32_t io_key_prefetch;
  int cached_errno;
  int32_t poll_timeout;
  int32_t connect_timeout;
  int32_t retry_timeout;
  uint32_t continuum_count; // Ketama
  int send_size;
  int recv_size;
  void *user_data;
  time_t next_distribution_rebuild; // Ketama
  size_t prefix_key_length;
  uint32_t number_of_replicas;
  hashkit_st distribution_hashkit;
  memcached_result_st result;
  memcached_continuum_item_st *continuum; // Ketama

  struct _allocators_st {
    memcached_calloc_fn calloc;
    memcached_free_fn free;
    memcached_malloc_fn malloc;
    memcached_realloc_fn realloc;
    void *context;
  } allocators;

  memcached_clone_fn on_clone;
  memcached_cleanup_fn on_cleanup;
  memcached_trigger_key_fn get_key_failure;
  memcached_trigger_delete_key_fn delete_trigger;
  memcached_callback_st *callbacks;
  char prefix_key[MEMCACHED_PREFIX_KEY_MAX_SIZE];
  struct {
    bool is_allocated:1;
  } options;

#ifdef __cplusplus
  memcached_st() :
    state(),
    flags(),
    distribution(),
    hashkit(),
    continuum_points_counter(),
    number_of_hosts(),
    servers(),
    last_disconnected_server(),
    snd_timeout(),
    rcv_timeout(),
    server_failure_limit(),
    io_msg_watermark(),
    io_bytes_watermark(),
    io_key_prefetch(),
    cached_errno(),
    poll_timeout(),
    connect_timeout(),
    retry_timeout(),
    continuum_count(),
    send_size(),
    recv_size(),
    user_data(),
    next_distribution_rebuild(),
    prefix_key_length(),
    number_of_replicas(),
    distribution_hashkit(),
    result(),
    continuum(),
    allocators(),
    on_clone(),
    on_cleanup(),
    get_key_failure(),
    delete_trigger(),
    callbacks(),
    prefix_key(),
    options()
  {
    memcached_create(this);
  }

  ~memcached_st()
  {
    memcached_free(this);
  }

  memcached_st(const memcached_st& source) :
    state(),
    flags(),
    distribution(),
    hashkit(),
    continuum_points_counter(),
    number_of_hosts(),
    servers(),
    last_disconnected_server(),
    snd_timeout(),
    rcv_timeout(),
    server_failure_limit(),
    io_msg_watermark(),
    io_bytes_watermark(),
    io_key_prefetch(),
    cached_errno(),
    poll_timeout(),
    connect_timeout(),
    retry_timeout(),
    continuum_count(),
    send_size(),
    recv_size(),
    user_data(),
    next_distribution_rebuild(),
    prefix_key_length(),
    number_of_replicas(),
    distribution_hashkit(),
    result(),
    continuum(),
    allocators(),
    on_clone(),
    on_cleanup(),
    get_key_failure(),
    delete_trigger(),
    callbacks(),
    prefix_key(),
    options()
   {
     memcached_clone(this, &source);
   }

  memcached_st& operator=(const memcached_st& source)
  { 
    memcached_clone(this, &source);

    return *this;
  }

#endif
};

// Local Only Inline
static inline memcached_server_st *memcached_server_instance_fetch(memcached_st *ptr, uint32_t server_key)
{
  return &ptr->servers[server_key];
}


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* __MEMCACHED_H__ */
