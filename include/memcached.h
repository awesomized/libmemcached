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
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memcached_st memcached_st;
typedef struct memcached_stat_st memcached_stat_st;
typedef struct memcached_result_st memcached_result_st;
typedef struct memcached_string_st memcached_string_st;
typedef struct memcached_server_st memcached_server_st;

#define MEMCACHED_VERSION_STRING 12
#define MEMCACHED_DEFAULT_PORT 11211
#define MEMCACHED_DEFAULT_COMMAND_SIZE 350
#define SMALL_STRING_LEN 1024
#define HUGE_STRING_LEN 8196
#define MEMCACHED_MAX_KEY 251 /* We add one to have it null terminated */
#define MEMCACHED_MAX_BUFFER HUGE_STRING_LEN

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
  MEMCACHED_STORED,
  MEMCACHED_NOTFOUND,
  MEMCACHED_MEMORY_ALLOCATION_FAILURE,
  MEMCACHED_PARTIAL_READ,
  MEMCACHED_SOME_ERRORS,
  MEMCACHED_NO_SERVERS,
  MEMCACHED_END,
  MEMCACHED_DELETED,
  MEMCACHED_VALUE,
  MEMCACHED_STAT,
  MEMCACHED_ERRNO,
  MEMCACHED_FAIL_UNIX_SOCKET,
  MEMCACHED_NOT_SUPPORTED,
  MEMCACHED_NO_KEY_PROVIDED,
  MEMCACHED_MAXIMUM_RETURN, /* Always add new error code before */
} memcached_return;

typedef enum {
  MEMCACHED_BEHAVIOR_NO_BLOCK,
  MEMCACHED_BEHAVIOR_TCP_NODELAY,
  MEMCACHED_BEHAVIOR_HASH,
  MEMCACHED_BEHAVIOR_KETAMA,
  MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE,
  MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE,
} memcached_behavior;

typedef enum {
  MEMCACHED_HASH_DEFAULT= 0,
  MEMCACHED_HASH_MD5,
  MEMCACHED_HASH_CRC,
  MEMCACHED_HASH_FNV1_64,
  MEMCACHED_HASH_FNV1A_64,
  MEMCACHED_HASH_FNV1_32,
  MEMCACHED_HASH_FNV1A_32,
  MEMCACHED_HASH_KETAMA,
} memcached_hash;

typedef enum {
  MEMCACHED_CONNECTION_UNKNOWN,
  MEMCACHED_CONNECTION_TCP,
  MEMCACHED_CONNECTION_UDP,
  MEMCACHED_CONNECTION_UNIX_SOCKET,
} memcached_connection;

typedef enum {
  MEMCACHED_NOT_ALLOCATED,
  MEMCACHED_ALLOCATED,
} memcached_allocated;

struct memcached_server_st {
  char *hostname;
  unsigned int port;
  int fd;
  unsigned int stack_responses;
  unsigned int cursor_active;
  char write_buffer[MEMCACHED_MAX_BUFFER];
  size_t write_buffer_offset;
  char *write_ptr;
  char read_buffer[MEMCACHED_MAX_BUFFER];
  size_t read_buffer_length;
  char *read_ptr;
  struct sockaddr_in servAddr;
  memcached_connection type;
  uint8_t major_version;
  uint8_t minor_version;
  uint8_t micro_version;
};

struct memcached_stat_st {
  unsigned int pid;
  unsigned int uptime;
  unsigned int threads;
  time_t time;
  char version[MEMCACHED_VERSION_STRING];
  unsigned int pointer_size;
  unsigned int rusage_user;
  unsigned int rusage_system;
  unsigned int rusage_user_seconds;
  unsigned int rusage_user_microseconds;
  unsigned int rusage_system_seconds;
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

struct memcached_string_st {
  memcached_st *root;
  memcached_allocated is_allocated;
  char *string;
  char *end;
  size_t current_size;
  size_t block_size;
};

struct memcached_result_st {
  memcached_allocated is_allocated;
  memcached_st *root;
  char key[MEMCACHED_MAX_KEY];
  size_t key_length;
  memcached_string_st value;
  uint16_t flags;
  uint64_t cas;
};

struct memcached_st {
  memcached_allocated is_allocated;
  memcached_server_st *hosts;
  unsigned int number_of_hosts;
  unsigned int cursor_server;
  char connected;
  int cached_errno;
  unsigned long long flags;
  int send_size;
  int recv_size;
  memcached_string_st result_buffer;
  memcached_hash hash;
  memcached_return warning; /* Future Use */
};

/* Public API */
memcached_st *memcached_create(memcached_st *ptr);
void memcached_free(memcached_st *ptr);
memcached_st *memcached_clone(memcached_st *clone, memcached_st *ptr);

memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration);
memcached_return memcached_increment(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     uint64_t *value);
memcached_return memcached_decrement(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     uint64_t *value);
void memcached_stat_free(memcached_st *, memcached_stat_st *);
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return *error);
memcached_return memcached_stat_servername(memcached_stat_st *stat, char *args, 
                                           char *hostname, unsigned int port);
memcached_return memcached_flush(memcached_st *ptr, time_t expiration);
memcached_return memcached_verbosity(memcached_st *ptr, unsigned int verbosity);
void memcached_quit(memcached_st *ptr);
char *memcached_strerror(memcached_st *ptr, memcached_return rc);
memcached_return memcached_behavior_set(memcached_st *ptr, memcached_behavior flag, void *data);
unsigned long long memcached_behavior_get(memcached_st *ptr, memcached_behavior flag);

/* All of the functions for adding data to the server */
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
memcached_return memcached_append(memcached_st *ptr, 
                                  char *key, size_t key_length,
                                  char *value, size_t value_length, 
                                  time_t expiration,
                                  uint16_t flags);
memcached_return memcached_prepend(memcached_st *ptr, 
                                   char *key, size_t key_length,
                                   char *value, size_t value_length, 
                                   time_t expiration,
                                   uint16_t flags);
memcached_return memcached_cas(memcached_st *ptr, 
                               char *key, size_t key_length,
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t flags,
                               uint64_t cas);

/* Get functions */
char *memcached_get(memcached_st *ptr, char *key, size_t key_length,
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error);
memcached_return memcached_mget(memcached_st *ptr, 
                                char **keys, size_t *key_length, 
                                unsigned int number_of_keys);
char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                      size_t *value_length, uint16_t *flags, 
                      memcached_return *error);
memcached_result_st *memcached_fetch_result(memcached_st *ptr, 
                                            memcached_result_st *result,
                                            memcached_return *error);

/* Server Public functions */
#define memcached_server_count(A) A->number_of_hosts
#define memcached_server_name(A,B) B.hostname
#define memcached_server_port(A,B) B.port
#define memcached_server_list(A) A->hosts
#define memcached_server_response_count(A,B) A->hosts[B].stack_responses

memcached_return memcached_server_add_udp(memcached_st *ptr, 
                                          char *hostname,
                                          unsigned int port);
memcached_return memcached_server_add_unix_socket(memcached_st *ptr, 
                                                  char *filename);
memcached_return memcached_server_add(memcached_st *ptr, char *hostname, 
                                      unsigned int port);
void memcached_server_list_free(memcached_server_st *ptr);
memcached_return memcached_server_push(memcached_st *ptr, memcached_server_st *list);

memcached_server_st *memcached_server_list_append(memcached_server_st *ptr, 
                                             char *hostname, unsigned int port, 
                                             memcached_return *error);
unsigned int memcached_server_list_count(memcached_server_st *ptr);
memcached_server_st *memcached_servers_parse(char *server_strings);

char *memcached_stat_get_value(memcached_st *ptr, memcached_stat_st *stat, 
                               char *key, memcached_return *error);
char ** memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *stat, 
                                memcached_return *error);

/* Result Struct */
void memcached_result_free(memcached_result_st *result);
memcached_result_st *memcached_result_create(memcached_st *ptr, 
                                             memcached_result_st *result);
#define memcached_result_key_value(A) A->key
#define memcached_result_key_length(A) A->key_length
#ifdef FIX
#define memcached_result_value(A) memcached_string_value(A->value)
#define memcached_result_length(A) memcached_string_length(A->value)
#else
char *memcached_result_value(memcached_result_st *ptr);
size_t memcached_result_length(memcached_result_st *ptr);
#endif
#define memcached_result_flags(A) A->flags
#define memcached_result_cas(A) A->cas


#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__
/* Some personal debugging functions */
#ifdef HAVE_DEBUG
#define WATCHPOINT fprintf(stderr, "\nWATCHPOINT %s:%d (%s)\n", __FILE__, __LINE__,__func__);fflush(stdout);
#ifdef __MEMCACHED_H__
#define WATCHPOINT_ERROR(A) fprintf(stderr, "\nWATCHPOINT %s:%d %s\n", __FILE__, __LINE__, memcached_strerror(NULL, A));fflush(stdout);
#endif
#define WATCHPOINT_STRING(A) fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s\n", __FILE__, __LINE__,__func__,A);fflush(stdout);
#define WATCHPOINT_NUMBER(A) fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %zu\n", __FILE__, __LINE__,__func__,(size_t)(A));fflush(stdout);
#define WATCHPOINT_ERRNO(A) fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s\n", __FILE__, __LINE__,__func__, strerror(A));A= 0;fflush(stdout);
#define WATCHPOINT_ASSERT(A) assert((A));
#else
/*
#define WATCHPOINT { 1; };
#define WATCHPOINT_ERROR(A) { 1; };
#define WATCHPOINT_STRING(A) { 1; };
#define WATCHPOINT_NUMBER(A) { 1; };
#define WATCHPOINT_ERRNO(A) { 1; };
#define WATCHPOINT_ASSERT(A) { 1; };
*/
#define WATCHPOINT
#ifdef __MEMCACHED_H__
#define WATCHPOINT_ERROR(A)
#endif
#define WATCHPOINT_STRING(A)
#define WATCHPOINT_NUMBER(A)
#define WATCHPOINT_ERRNO(A)
#define WATCHPOINT_ASSERT(A)
#endif

#endif /* __WATCHPOINT_H__ */


#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_H__ */
