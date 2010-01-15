/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary:
 *
 */

/*
  Common include file for libmemached
*/

#ifndef LIBMEMCACHED_COMMON_H
#define LIBMEMCACHED_COMMON_H

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

/* Define this here, which will turn on the visibilty controls while we're
 * building libmemcached.
 */
#define BUILDING_LIBMEMCACHED 1


#include "libmemcached/memcached.h"
#include "libmemcached/watchpoint.h"

typedef struct memcached_server_st memcached_server_instance_st;

/* These are private not to be installed headers */
#include "libmemcached/io.h"
#include "libmemcached/internal.h"
#include "libmemcached/libmemcached_probes.h"
#include "libmemcached/memcached/protocol_binary.h"
#include "libmemcached/byteorder.h"
#include "libmemcached/response.h"

/* string value */
struct memcached_continuum_item_st
{
  uint32_t index;
  uint32_t value;
};

/* Yum, Fortran.... can you make the reference? */
typedef enum {
  MEM_NOT= -1,
  MEM_FALSE= false,
  MEM_TRUE= true,
} memcached_ternary_t;


#if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)

#define likely(x)       if((x))
#define unlikely(x)     if((x))

#else

#define likely(x)       if(__builtin_expect((x) != 0, 1))
#define unlikely(x)     if(__builtin_expect((x) != 0, 0))
#endif

#define MEMCACHED_BLOCK_SIZE 1024
#define MEMCACHED_DEFAULT_COMMAND_SIZE 350
#define SMALL_STRING_LEN 1024
#define HUGE_STRING_LEN 8196

/* Hashing algo */

LIBMEMCACHED_LOCAL
void md5_signature(const unsigned char *key, unsigned int length, unsigned char *result);
LIBMEMCACHED_LOCAL
uint32_t hash_crc32(const char *data,
                    size_t data_len);
#ifdef HAVE_HSIEH_HASH
LIBMEMCACHED_LOCAL
uint32_t hsieh_hash(const char *key, size_t key_length);
#endif
LIBMEMCACHED_LOCAL
uint32_t murmur_hash(const char *key, size_t key_length);
LIBMEMCACHED_LOCAL
uint32_t jenkins_hash(const void *key, size_t length, uint32_t initval);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_connect(memcached_server_instance_st *ptr);
LIBMEMCACHED_LOCAL
void memcached_quit_server(memcached_server_instance_st *ptr, uint8_t io_death);

#define memcached_server_response_increment(A) (A)->cursor_active++
#define memcached_server_response_decrement(A) (A)->cursor_active--
#define memcached_server_response_reset(A) (A)->cursor_active=0

LIBMEMCACHED_LOCAL
memcached_return_t memcached_do(memcached_server_instance_st *ptr, const void *commmand,
                                size_t command_length, uint8_t with_flush);
LIBMEMCACHED_LOCAL
memcached_return_t value_fetch(memcached_server_instance_st *ptr,
                               char *buffer,
                               memcached_result_st *result);
LIBMEMCACHED_LOCAL
void server_list_free(memcached_st *ptr, memcached_server_instance_st *servers);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_key_test(const char * const *keys,
                                      const size_t *key_length,
                                      size_t number_of_keys);


LIBMEMCACHED_LOCAL
uint32_t generate_hash(memcached_st *ptr, const char *key, size_t key_length);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_purge(memcached_server_instance_st *ptr);

static inline memcached_return_t memcached_validate_key_length(size_t key_length, bool binary)
{
  unlikely (key_length == 0)
    return MEMCACHED_BAD_KEY_PROVIDED;

  if (binary)
  {
    unlikely (key_length > 0xffff)
      return MEMCACHED_BAD_KEY_PROVIDED;
  }
  else
  {
    unlikely (key_length >= MEMCACHED_MAX_KEY)
      return MEMCACHED_BAD_KEY_PROVIDED;
  }

  return MEMCACHED_SUCCESS;
}

#ifdef TCP_CORK
  #define CORK TCP_CORK
#elif defined TCP_NOPUSH
  #define CORK TCP_NOPUSH
#endif

/*
  cork_switch() tries to enable TCP_CORK. IF TCP_CORK is not an option
  on the system it returns false but sets errno to 0. Otherwise on
  failure errno is set.
*/
static inline memcached_ternary_t cork_switch(memcached_server_st *ptr, bool enable)
{
#ifdef CORK
  if (ptr->type != MEMCACHED_CONNECTION_TCP)
    return MEM_FALSE;

  int err= setsockopt(ptr->fd, IPPROTO_TCP, CORK,
                      &enable, (socklen_t)sizeof(int));
  if (! err)
  {
    return MEM_TRUE;
  }
  else
  {
    ptr->cached_errno= errno;
    return MEM_FALSE;
  }
#else
  (void)ptr;
  (void)enable;

  ptr->cached_errno= 0;

  return MEM_NOT;
#endif
}

#endif /* LIBMEMCACHED_COMMON_H */
