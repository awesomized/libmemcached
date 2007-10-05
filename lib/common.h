/*
  Common include file for libmemached
*/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <memcached.h>
#include <memcached_io.h>

#include <libmemcached_config.h>

#ifdef HAVE_DTRACE
#define _DTRACE_VERSION 1
#else
#undef _DTRACE_VERSION
#endif

#include "libmemcached_probes.h"

void md5_signature(const unsigned char *key, unsigned int length, char *result);
memcached_return memcached_connect(memcached_st *ptr);
memcached_return memcached_response(memcached_st *ptr, 
                                    char *buffer, size_t buffer_length,
                                    unsigned int server_key);
unsigned int memcached_generate_hash(char *key, size_t key_length);
char *memcached_stat_get_value(memcached_st *ptr, memcached_stat_st *stat, 
                               char *key, memcached_return *error);
char ** memcached_stat_get_keys(memcached_st *ptr, memcached_stat_st *stat, 
                                memcached_return *error);

#endif /* __COMMON_H__ */
