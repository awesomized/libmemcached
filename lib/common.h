/*
  Common include file for libmemached
*/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#include "libmemcached_config.h"

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif



#include <memcached.h>
#include "memcached_io.h"

#include <libmemcached_config.h>

#ifdef HAVE_DTRACE
#define _DTRACE_VERSION 1
#else
#undef _DTRACE_VERSION
#endif

#include "libmemcached_probes.h"

#define MEMCACHED_BLOCK_SIZE 1024

typedef enum {
  MEM_NO_BLOCK= (1 << 0),
  MEM_TCP_NODELAY= (1 << 1),
  MEM_REUSE_MEMORY= (1 << 2),
  MEM_USE_MD5= (1 << 3),
  MEM_USE_KETAMA= (1 << 4),
  MEM_USE_CRC= (1 << 5),
  MEM_USE_CACHE_LOOKUPS= (1 << 6),
  MEM_SUPPORT_CAS= (1 << 7),
  MEM_BUFFER_REQUESTS= (1 << 8),
} memcached_flags;

/* Hashing algo */
void md5_signature(unsigned char *key, unsigned int length, unsigned char *result);
uint32_t hash_crc32(const char *data,
                    size_t data_len);
uint32_t hsieh_hash(char *key, size_t key_length);

memcached_return memcached_connect(memcached_st *ptr, unsigned int server_key);
memcached_return memcached_response(memcached_st *ptr, 
                                    char *buffer, size_t buffer_length,
                                    memcached_result_st *result,
                                    unsigned int server_key);
unsigned int memcached_generate_hash(memcached_st *ptr, char *key, size_t key_length);
void memcached_quit_server(memcached_st *ptr, unsigned int server_key, uint8_t io_death);

#define memcached_server_response_increment(A,B) A->hosts[B].cursor_active++
#define memcached_server_response_decrement(A,B) A->hosts[B].cursor_active--
#define memcached_server_response_reset(A,B) A->hosts[B].cursor_active=0

/* String Struct */
#define memcached_string_length(A) (size_t)((A)->end - (A)->string)
#define memcached_string_set_length(A, B) (A)->end= (A)->string + B
#define memcached_string_size(A) (A)->current_size
#define memcached_string_value(A) (A)->string

memcached_string_st *memcached_string_create(memcached_st *ptr, 
                                             memcached_string_st *string, 
                                             size_t initial_size);
memcached_return memcached_string_check(memcached_string_st *string, size_t need);
char *memcached_string_c_copy(memcached_string_st *string);
memcached_return memcached_string_append_character(memcached_string_st *string, 
                                                   char character);
memcached_return memcached_string_append(memcached_string_st *string,
                                         char *value, size_t length);
size_t memcached_string_backspace(memcached_string_st *string, size_t remove);
memcached_return memcached_string_reset(memcached_string_st *string);
void memcached_string_free(memcached_string_st *string);
memcached_return memcached_do(memcached_st *ptr, unsigned int server_key, char *commmand, 
                              size_t command_length, char with_flush);
memcached_return memcached_version(memcached_st *ptr);
memcached_return value_fetch(memcached_st *ptr,
                             char *buffer,
                             memcached_result_st *result,
                             unsigned int server_key);


#endif /* __COMMON_H__ */
