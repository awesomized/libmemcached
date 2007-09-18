/*
 * Summary: interface for memcached server
 * Description: main include file for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Brian Aker
 */

#ifndef __MEMCACHED_H__
#define __MEMCACHED_H__

#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memcached_st memcached_st;
typedef struct memcached_stat_st memcached_stat_st;

#define MEMCACHED_DEFAULT_PORT 11211
#define MEMCACHED_DEFAULT_COMMAND_SIZE 350

typedef enum {
  MEMCACHED_SUCCESS,
  MEMCACHED_FAILURE,
  MEMCACHED_HOST_LOCKUP_FAILURE,
  MEMCACHED_CONNECTION_FAILURE,
  MEMCACHED_CONNECTION_BIND_FAILURE,
  MEMCACHED_WRITE_FAILURE,
  MEMCACHED_READ_FAILURE,
  MEMCACHED_UNKNOWN_READ_FAILURE,
  MEMCACHED_PROTOCOL_ERROR,
  MEMCACHED_CLIENT_ERROR,
  MEMCACHED_SERVER_ERROR,
  MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE,
  MEMCACHED_DATA_EXISTS,
  MEMCACHED_DATA_DOES_NOT_EXIST,
  MEMCACHED_NOTSTORED,
  MEMCACHED_NOTFOUND,
  MEMCACHED_MEMORY_ALLOCATION_FAILURE,
  MEMCACHED_PARTIAL_READ,
} memcached_return;

typedef enum {
  MEMCACHED_NOT_ALLOCATED= 0,
  MEMCACHED_ALLOCATED= 1,
} memcached_allocated;

struct memcached_stat_st {
  unsigned int pid;
  unsigned int uptime;
  time_t time;
  char *version;
  unsigned rusage_user_seconds;
  unsigned int rusage_user_microseconds;
  unsigned rusage_system_seconds;
  unsigned int rusage_system_microseconds;
  unsigned int curr_items;
  unsigned int total_items;
  unsigned long long bytes;
  unsigned int curr_connections;
  unsigned int total_connections;
  unsigned int connection_structures;
  unsigned long long cmd_get;
  unsigned long long cmd_set;
  unsigned long long get_hits;
  unsigned long long get_misses;
  unsigned long long evictions;
  unsigned long long bytes_read;
  unsigned long long bytes_written;
  unsigned int limit_maxbytes;
};

struct memcached_st {
  memcached_allocated is_allocated;
  int fd;
  char connected;
};

/* Public API */
memcached_st *memcached_init(memcached_st *ptr);
void memcached_deinit(memcached_st *ptr);

memcached_return memcached_set(memcached_st *ptr, char *key, size_t key_length, 
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags);
memcached_return memcached_add(memcached_st *ptr, char *key, size_t key_length,
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags);
memcached_return memcached_replace(memcached_st *ptr, char *key, size_t key_length,
                                   char *value, size_t value_length, 
                                   time_t expiration,
                                   uint16_t  flags);
memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration);
memcached_return memcached_increment(memcached_st *ptr, char *key, size_t key_length,
                                     unsigned int count);
memcached_return memcached_decrement(memcached_st *ptr, char *key, size_t key_length,
                                     unsigned int count);
memcached_return memcached_stat(memcached_st *ptr, memcached_stat_st *stat);
memcached_return memcached_flush(memcached_st *ptr, time_t expiration);
char *memcached_version(memcached_st *ptr, memcached_return *error);
memcached_return memcached_verbosity(memcached_st *ptr, unsigned int verbosity);
memcached_return memcached_quit(memcached_st *ptr);
char *memcached_get(memcached_st *ptr, char *key, size_t key_length,
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error);
void memcached_server_add(memcached_st *ptr, char *server_name, unsigned int port);
static memcached_return memcached_response(memcached_st *ptr, 
                                           char *buffer, size_t buffer_length);
char *memcached_strerror(memcached_st *ptr, memcached_return rc);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_H__ */
