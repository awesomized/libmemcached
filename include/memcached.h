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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct memcached_st memcached_st;
typedef struct memcached_stat_st memcached_stat_st;
typedef struct memcached_string_st memcached_string_st;
typedef struct memcached_server_st memcached_server_st;

#define MEMCACHED_DEFAULT_PORT 11211
#define MEMCACHED_DEFAULT_COMMAND_SIZE 350
#define SMALL_STRING_LEN 1024
#define HUGE_STRING_LEN 8196
#define MEMCACHED_MAX_KEY 251 /* We add one to have it null terminated */
//#define MEMCACHED_MAX_BUFFER 8196
#define MEMCACHED_MAX_BUFFER 8196

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
  MEMCACHED_SOME_ERRORS,
  MEMCACHED_NO_SERVERS,
  MEMCACHED_END,
  MEMCACHED_DELETED,
  MEMCACHED_VALUE,
  MEMCACHED_MAXIMUM_RETURN, /* Always add new error code before */
} memcached_return;

typedef enum {
  MEMCACHED_NOT_ALLOCATED= 0,
  MEMCACHED_ALLOCATED= 1,
} memcached_allocated;

struct memcached_server_st {
  char *hostname;
  unsigned int port;
  int fd;
};

struct memcached_stat_st {
  unsigned int pid;
  unsigned int uptime;
  unsigned int threads;
  time_t time;
  char version[8];
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
  char *string;
  char *end;
  size_t current_size;
  size_t block_size;
};

struct memcached_st {
  memcached_allocated is_allocated;
  memcached_server_st *hosts;
  unsigned int number_of_hosts;
  unsigned int cursor_server;
  char read_buffer[MEMCACHED_MAX_BUFFER];
  size_t read_buffer_length;
  char *read_ptr;
  char write_buffer[MEMCACHED_MAX_BUFFER];
  size_t write_buffer_offset;
  size_t write_between_flush;
  char connected;
};

/* Public API */
memcached_st *memcached_create(memcached_st *ptr);
void memcached_free(memcached_st *ptr);

memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration);
memcached_return memcached_increment(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     unsigned int *value);
memcached_return memcached_decrement(memcached_st *ptr, 
                                     char *key, size_t key_length,
                                     unsigned int offset,
                                     unsigned int *value);
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return *error);
memcached_return memcached_stat_servername(memcached_stat_st *stat, char *args, 
                                           char *hostname, unsigned int port);
memcached_return memcached_flush(memcached_st *ptr, time_t expiration);
memcached_return memcached_verbosity(memcached_st *ptr, unsigned int verbosity);
void memcached_quit(memcached_st *ptr);
char *memcached_strerror(memcached_st *ptr, memcached_return rc);

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

/* Server Public functions */
#define memcached_server_count(A) A->number_of_hosts
#define memcached_server_name(A,B) B.hostname
#define memcached_server_port(A,B) B.port
#define memcached_server_list(A) A->hosts

memcached_return memcached_server_add(memcached_st *ptr, char *hostname, 
                                      unsigned int port);

memcached_server_st *memcached_server_list_append(memcached_server_st *ptr, 
                                             char *hostname, unsigned int port, 
                                             memcached_return *error);
void memcached_server_list_free(memcached_server_st *ptr);
memcached_return memcached_server_push(memcached_st *ptr, memcached_server_st *list);
unsigned int memcached_server_list_count(memcached_server_st *ptr);

/* These are all private, do not use. */
memcached_return memcached_connect(memcached_st *ptr);
memcached_return memcached_response(memcached_st *ptr, 
                                    char *buffer, size_t buffer_length,
                                    unsigned int server_key);
unsigned int memcached_generate_hash(char *key, size_t key_length);
char *memcached_stat_get_value(memcached_st *ptr, memcached_stat_st *stat, 
                               char *key, memcached_return *error);
char ** memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *stat, 
                                memcached_return *error);


/* String Struct */
#define memcached_string_length(A, B) (size_t)(B->end - B->string)
#define memcached_string_size(A, B) B->current_size
#define memcached_string_value(A, B) B->string

memcached_string_st *memcached_string_create(memcached_st *ptr, size_t initial_size);
memcached_return memcached_string_append_character(memcached_st *ptr, 
                                                   memcached_string_st *string, 
                                                   char character);
memcached_return memcached_string_append(memcached_st *ptr, memcached_string_st *string,
                                         char *value, size_t length);
size_t memcached_string_backspace(memcached_st *ptr, memcached_string_st *string, size_t remove);
memcached_return memcached_string_reset(memcached_st *ptr, memcached_string_st *string);
void memcached_string_free(memcached_st *ptr, memcached_string_st *string);

/* Some personal debugging functions */
#define WATCHPOINT printf("WATCHPOINT %s:%d\n", __FILE__, __LINE__);fflush(stdout);
#define WATCHPOINT_ERROR(A) printf("WATCHPOINT %s:%d %s\n", __FILE__, __LINE__, memcached_strerror(NULL, A));fflush(stdout);
#define WATCHPOINT_STRING(A) printf("WATCHPOINT %s:%d %s\n", __FILE__, __LINE__, A);fflush(stdout);
#define WATCHPOINT_NUMBER(A) printf("WATCHPOINT %s:%d %d\n", __FILE__, __LINE__, A);fflush(stdout);


#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_H__ */
