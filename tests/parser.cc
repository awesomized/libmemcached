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

  memcached_free(memc);

  return TEST_SUCCESS;
}
