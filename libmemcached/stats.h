/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker All rights reserved.
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

#pragma once


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
  memcached_st *root;
};

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
void memcached_stat_free(const memcached_st *, memcached_stat_st *);

LIBMEMCACHED_API
memcached_stat_st *memcached_stat(memcached_st *ptr, char *args, memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_stat_servername(memcached_stat_st *memc_stat, char *args,
                                             const char *hostname, in_port_t port);

LIBMEMCACHED_API
char *memcached_stat_get_value(const memcached_st *ptr, memcached_stat_st *memc_stat,
                               const char *key, memcached_return_t *error);

LIBMEMCACHED_API
char ** memcached_stat_get_keys(const memcached_st *ptr, memcached_stat_st *memc_stat,
                                memcached_return_t *error);

LIBMEMCACHED_API
memcached_return_t memcached_stat_execute(memcached_st *memc, const char *args,  memcached_stat_fn func, void *context);

#ifdef __cplusplus
} // extern "C"
#endif
