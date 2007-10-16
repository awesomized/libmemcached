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
#include <time.h>
#include <errno.h>


#include <memcached.h>
#include <memcached_io.h>

#include <libmemcached_config.h>

#ifdef HAVE_DTRACE
#define _DTRACE_VERSION 1
#else
#undef _DTRACE_VERSION
#endif

#include "libmemcached_probes.h"

typedef enum {
  MEM_NO_BLOCK= (1 << 0),
  MEM_TCP_NODELAY= (1 << 1),
  MEM_REUSE_MEMORY= (1 << 2),
  MEM_USE_MD5= (1 << 3),
  MEM_USE_KETAMA= (1 << 4),
} memcached_flags;

void md5_signature(unsigned char *key, unsigned int length, unsigned char *result);
memcached_return memcached_connect(memcached_st *ptr, unsigned int server_key);
memcached_return memcached_response(memcached_st *ptr, 
                                    char *buffer, size_t buffer_length,
                                    unsigned int server_key);
unsigned int memcached_generate_hash(memcached_st *ptr, char *key, size_t key_length);
void memcached_quit_server(memcached_st *ptr, unsigned int server_key);

#define memcached_server_response_increment(A,B) A->hosts[B].stack_responses++
#define memcached_server_response_decrement(A,B) A->hosts[B].stack_responses--

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


#endif /* __COMMON_H__ */
