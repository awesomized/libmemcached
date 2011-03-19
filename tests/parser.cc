/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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

#include <config.h>

#include <libmemcached/memcached.h>

#include "tests/parser.h"
#include "tests/print.h"

struct scanner_string_st {
  const char *c_ptr;
  size_t size;
};

test_return_t server_test(memcached_st *junk)
{
  (void)junk;
  memcached_return_t rc;
  memcached_st *memc;
  memc= memcached_create(NULL);

  scanner_string_st test_strings[]= {
    { STRING_WITH_LEN("--server=localhost") },
    { STRING_WITH_LEN("--server=10.0.2.1") },
    { STRING_WITH_LEN("--server=example.com") },
    { STRING_WITH_LEN("--server=localhost:30") },
    { STRING_WITH_LEN("--server=10.0.2.1:20") },
    { STRING_WITH_LEN("--server=example.com:1024") },
    { NULL, 0}
  };

  for (scanner_string_st *ptr= test_strings; ptr->size; ptr++)
  {
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);
    test_true(rc == MEMCACHED_SUCCESS);
    memcached_servers_reset(memc);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

test_return_t servers_test(memcached_st *junk)
{
  (void)junk;
  memcached_st *memc;
  memc= memcached_create(NULL);

  scanner_string_st test_strings[]= {
    { STRING_WITH_LEN("--servers=localhost:11221,localhost:11222,localhost:11223,localhost:11224,localhost:11225") },
    { STRING_WITH_LEN("--servers=a.example.com:81,localhost:82,b.example.com") },
    { STRING_WITH_LEN("--servers=localhost,localhost:80") },
    { NULL, 0}
  };

  for (scanner_string_st *ptr= test_strings; ptr->size; ptr++)
  {
    memcached_return_t rc;
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);

    test_true(rc == MEMCACHED_SUCCESS);

    memcached_server_fn callbacks[1];
    callbacks[0]= server_print_callback;
    memcached_server_cursor(memc, callbacks, NULL,  1);

    memcached_servers_reset(memc);
  }

  scanner_string_st bad_test_strings[]= {
    { STRING_WITH_LEN("-servers=localhost:11221,localhost:11222,localhost:11223,localhost:11224,localhost:11225") },
    { STRING_WITH_LEN("-- servers=a.example.com:81,localhost:82,b.example.com") },
    { STRING_WITH_LEN("--servers=localhost80") },
    { NULL, 0}
  };

  for (scanner_string_st *ptr= bad_test_strings; ptr->size; ptr++)
  {
    memcached_return_t rc;
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);

    test_false_with(rc == MEMCACHED_SUCCESS, ptr->c_ptr);

    memcached_server_fn callbacks[1];
    callbacks[0]= server_print_callback;
    memcached_server_cursor(memc, callbacks, NULL,  1);

    memcached_servers_reset(memc);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

scanner_string_st test_number_options[]= {
  { STRING_WITH_LEN("--CONNECT_TIMEOUT=456") },
  { STRING_WITH_LEN("--IO_MSG_WATERMARK=456") },
  { STRING_WITH_LEN("--IO_BYTES_WATERMARK=456") },
  { STRING_WITH_LEN("--IO_KEY_PREFETCH=456") },
  { STRING_WITH_LEN("--NUMBER_OF_REPLICAS=456") },
  { STRING_WITH_LEN("--POLL_TIMEOUT=456") },
  { STRING_WITH_LEN("--RCV_TIMEOUT=456") },
  { STRING_WITH_LEN("--RETRY_TIMEOUT=456") },
  { STRING_WITH_LEN("--SERVER_FAILURE_LIMIT=456") },
  { STRING_WITH_LEN("--SND_TIMEOUT=456") },
  { STRING_WITH_LEN("--SOCKET_RECV_SIZE=456") },
  { STRING_WITH_LEN("--SOCKET_SEND_SIZE=456") },
  { NULL, 0}
};

scanner_string_st test_boolean_options[]= {
  { STRING_WITH_LEN("--AUTO_EJECT_HOSTS") },
  { STRING_WITH_LEN("--BINARY_PROTOCOL") },
  { STRING_WITH_LEN("--BUFFER_REQUESTS") },
  { STRING_WITH_LEN("--CACHE_LOOKUPS") },
  { STRING_WITH_LEN("--CORK") },
  { STRING_WITH_LEN("--HASH_WITH_PREFIX_KEY") },
  { STRING_WITH_LEN("--KETAMA") },
  { STRING_WITH_LEN("--KETAMA_WEIGHTED") },
  { STRING_WITH_LEN("--NOREPLY") },
  { STRING_WITH_LEN("--RANDOMIZE_REPLICA_READ") },
  { STRING_WITH_LEN("--SORT_HOSTS") },
  { STRING_WITH_LEN("--SUPPORT_CAS") },
  { STRING_WITH_LEN("--TCP_NODELAY") },
  { STRING_WITH_LEN("--TCP_KEEPALIVE") },
  { STRING_WITH_LEN("--TCP_KEEPIDLE") },
  { STRING_WITH_LEN("--USE_UDP") },
  { STRING_WITH_LEN("--VERIFY_KEY") },
  { NULL, 0}
};

test_return_t parser_number_options_test(memcached_st *junk)
{
  (void)junk;
  memcached_st *memc;
  memc= memcached_create(NULL);

  for (scanner_string_st *ptr= test_number_options; ptr->size; ptr++)
  {
    memcached_return_t rc;
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);
    test_true_got(rc == MEMCACHED_SUCCESS, ptr->c_ptr);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

test_return_t parser_boolean_options_test(memcached_st *junk)
{
  (void)junk;
  memcached_st *memc;
  memc= memcached_create(NULL);

  for (scanner_string_st *ptr= test_boolean_options; ptr->size; ptr++)
  {
    memcached_return_t rc;
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);
    test_true_got(rc == MEMCACHED_SUCCESS, ptr->c_ptr);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

test_return_t behavior_parser_test(memcached_st *junk)
{
  (void)junk;
  return TEST_SUCCESS;
}

test_return_t parser_hash_test(memcached_st *junk)
{
  (void)junk;
  memcached_return_t rc;
  memcached_st *memc;
  memc= memcached_create(NULL);

  scanner_string_st test_strings[]= {
    { STRING_WITH_LEN("--HASH=MD5") },
    { STRING_WITH_LEN("--HASH=CRC") },
    { STRING_WITH_LEN("--HASH=FNV1_64") },
    { STRING_WITH_LEN("--HASH=FNV1A_64") },
    { STRING_WITH_LEN("--HASH=FNV1_32") },
    { STRING_WITH_LEN("--HASH=FNV1A_32") },
    { STRING_WITH_LEN("--HASH=HSIEH") },
    { STRING_WITH_LEN("--HASH=MURMUR") },
    { STRING_WITH_LEN("--HASH=JENKINS") },
    { NULL, 0}
  };

  for (scanner_string_st *ptr= test_strings; ptr->size; ptr++)
  {
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);
    test_true_got(rc == MEMCACHED_SUCCESS, ptr->c_ptr);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

test_return_t parser_distribution_test(memcached_st *junk)
{
  (void)junk;
  memcached_return_t rc;
  memcached_st *memc;
  memc= memcached_create(NULL);

  scanner_string_st test_strings[]= {
    { STRING_WITH_LEN("--DISTRIBUTION=consistent") },
    { STRING_WITH_LEN("--DISTRIBUTION=random") },
    { STRING_WITH_LEN("--DISTRIBUTION=modula") },
    { NULL, 0}
  };

  for (scanner_string_st *ptr= test_strings; ptr->size; ptr++)
  {
    rc= memcached_parse_options(memc, ptr->c_ptr, ptr->size);
    test_true_got(rc == MEMCACHED_SUCCESS, ptr->c_ptr);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}
