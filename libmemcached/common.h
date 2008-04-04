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

#if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)

#define likely(x)       if((x))
#define unlikely(x)     if((x))

#else

#define likely(x)       if(__builtin_expect(!!(x), 1))
#define unlikely(x)     if(__builtin_expect((x), 0))
#endif


#ifdef HAVE_DTRACE
#define _DTRACE_VERSION 1
#else
#undef _DTRACE_VERSION
#endif

#include "libmemcached_probes.h"

#define MEMCACHED_BLOCK_SIZE 1024
#define MEMCACHED_DEFAULT_COMMAND_SIZE 350
#define SMALL_STRING_LEN 1024
#define HUGE_STRING_LEN 8196


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
  MEM_USE_SORT_HOSTS= (1 << 9),
  MEM_VERIFY_KEY= (1 << 10),
} memcached_flags;

/* Hashing algo */
void md5_signature(unsigned char *key, unsigned int length, unsigned char *result);
uint32_t hash_crc32(const char *data,
                    size_t data_len);
uint32_t hsieh_hash(char *key, size_t key_length);
uint32_t murmur_hash(char *key, size_t key_length);

memcached_return memcached_connect(memcached_server_st *ptr);
memcached_return memcached_response(memcached_server_st *ptr,
                                    char *buffer, size_t buffer_length,
                                    memcached_result_st *result);
unsigned int memcached_generate_hash(memcached_st *ptr, char *key, size_t key_length);
void memcached_quit_server(memcached_server_st *ptr, uint8_t io_death);

#define memcached_server_response_increment(A) (A)->cursor_active++
#define memcached_server_response_decrement(A) (A)->cursor_active--
#define memcached_server_response_reset(A) (A)->cursor_active=0

memcached_return memcached_do(memcached_server_st *ptr, char *commmand,
                              size_t command_length, uint8_t with_flush);
memcached_return memcached_version(memcached_st *ptr);
memcached_return value_fetch(memcached_server_st *ptr,
                             char *buffer,
                             memcached_result_st *result);
void server_list_free(memcached_st *ptr, memcached_server_st *servers);

memcached_return memcachd_key_test(char **keys, size_t *key_length,
                                   unsigned int number_of_keys);

void sort_hosts(memcached_st *ptr);

#endif /* __COMMON_H__ */
