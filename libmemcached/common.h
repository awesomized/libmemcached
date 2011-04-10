/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  LibMemcached
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
  Common include file for libmemached
*/

#pragma once

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
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


#include <libmemcached/memcached.h>
#include <libmemcached/watchpoint.h>
#include <libmemcached/is.h>
#include <libmemcached/prefix_key.h>

typedef struct memcached_server_st * memcached_server_write_instance_st;

typedef memcached_return_t (*memcached_server_execute_fn)(memcached_st *ptr, memcached_server_write_instance_st server, void *context);

LIBMEMCACHED_LOCAL
memcached_server_write_instance_st memcached_server_instance_fetch(memcached_st *ptr, uint32_t server_key);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_server_execute(memcached_st *ptr,
                                            memcached_server_execute_fn callback,
                                            void *context);


/* These are private not to be installed headers */
#include <libmemcached/io.h>
#include <libmemcached/do.h>
#include <libmemcached/internal.h>
#include <libmemcached/array.h>
#include <libmemcached/libmemcached_probes.h>
#include <libmemcached/memcached/protocol_binary.h>
#include <libmemcached/byteorder.h>
#include <libmemcached/initialize_query.h>
#include <libmemcached/response.h>
#include <libmemcached/prefix_key.h>

/* string value */
struct memcached_continuum_item_st
{
  uint32_t index;
  uint32_t value;
};

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

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_LOCAL
memcached_return_t memcached_connect(memcached_server_write_instance_st ptr);

LIBMEMCACHED_LOCAL
memcached_return_t run_distribution(memcached_st *ptr);

#define memcached_server_response_increment(A) (A)->cursor_active++
#define memcached_server_response_decrement(A) (A)->cursor_active--
#define memcached_server_response_reset(A) (A)->cursor_active=0

LIBMEMCACHED_LOCAL
void set_last_disconnected_host(memcached_server_write_instance_st ptr);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_key_test(const char * const *keys,
                                      const size_t *key_length,
                                      size_t number_of_keys);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_purge(memcached_server_write_instance_st ptr);

LIBMEMCACHED_LOCAL
memcached_server_st *memcached_server_create_with(const memcached_st *memc,
                                                  memcached_server_write_instance_st host,
                                                  const char *hostname,
                                                  in_port_t port,
                                                  uint32_t weight,
                                                  memcached_connection_t type);


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

static inline void libmemcached_free(const memcached_st *ptr, void *mem)
{
  ptr->allocators.free(ptr, mem, ptr->allocators.context);
}

static inline void *libmemcached_malloc(const memcached_st *ptr, const size_t size)
{
  return ptr->allocators.malloc(ptr, size, ptr->allocators.context);
}

static inline void *libmemcached_realloc(const memcached_st *ptr, void *mem, const size_t size)
{
  return ptr->allocators.realloc(ptr, mem, size, ptr->allocators.context);
}

static inline void *libmemcached_calloc(const memcached_st *ptr, size_t nelem, size_t size)
{
  return ptr->allocators.calloc(ptr, nelem, size, ptr->allocators.context);
}

#ifdef __cplusplus
}
#endif
