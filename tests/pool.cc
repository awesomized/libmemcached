/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached Client and Server 
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

#include <vector>
#include <iostream>
#include <string>
#include <errno.h>

#include <libmemcached/memcached.h>
#include <libmemcached/util.h>
#include <tests/pool.h>

test_return_t memcached_pool_test(memcached_st *)
{
  memcached_return_t rc;
  const char *config_string= "--SERVER=host10.example.com --SERVER=host11.example.com --SERVER=host10.example.com --POOL-MIN=10 --POOL-MAX=32";

  char buffer[2048];
  rc= libmemcached_check_configuration(config_string, sizeof(config_string) -1, buffer, sizeof(buffer));

  test_true_got(rc != MEMCACHED_SUCCESS, buffer);

  memcached_pool_st* pool= memcached_pool(config_string, strlen(config_string));
  test_true_got(pool, strerror(errno));

  memcached_st *memc= memcached_pool_pop(pool, false, &rc);

  test_true(rc == MEMCACHED_SUCCESS);
  test_true(memc);

  /*
    Release the memc_ptr that was pulled from the pool
  */
  memcached_pool_push(pool, memc);

  /*
    Destroy the pool.
  */
  memcached_pool_destroy(pool);

  return TEST_SUCCESS;
}
