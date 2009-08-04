/*
 * Summary: Stat functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_STATS_H
#define LIBMEMCACHED_MEMCACHED_STATS_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

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

LIBMEMCACHED_API
void memcached_stat_free(memcached_st *, memcached_stat_st *);
LIBMEMCACHED_API
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return *error);
LIBMEMCACHED_API
memcached_return memcached_stat_servername(memcached_stat_st *memc_stat, char *args, 
                                           char *hostname, unsigned int port);
LIBMEMCACHED_API
char *memcached_stat_get_value(memcached_st *ptr, memcached_stat_st *memc_stat, 
                               const char *key, memcached_return *error);
LIBMEMCACHED_API
char ** memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *memc_stat, 
                                memcached_return *error);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_STATS_H */
