/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Collect up the stats for a memcached server.
 *
 */

#ifndef __MEMCACHED_STATS_H__
#define __MEMCACHED_STATS_H__

#ifdef __cplusplus
extern "C" {
#endif

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
  memcached_st *root;
};

LIBMEMCACHED_API
void memcached_stat_free(memcached_st *, memcached_stat_st *);

LIBMEMCACHED_API
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_stat_servername(memcached_stat_st *memc_stat, char *args,
                                             const char *hostname, in_port_t port);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_STATS_H__ */
