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

#include <libtest/test.h>
#include "libmemcached/common.h"
#include <tests/replication.h>

test_return_t replication_set_test(memcached_st *memc)
{
  memcached_return_t rc;
  memcached_st *memc_clone= memcached_clone(NULL, memc);
  memcached_behavior_set(memc_clone, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 0);

  rc= memcached_set(memc, "bubba", 5, "0", 1, 0, 0);
  test_true(rc == MEMCACHED_SUCCESS);

  /*
  ** We are using the quiet commands to store the replicas, so we need
  ** to ensure that all of them are processed before we can continue.
  ** In the test we go directly from storing the object to trying to
  ** receive the object from all of the different servers, so we
  ** could end up in a race condition (the memcached server hasn't yet
  ** processed the quiet command from the replication set when it process
  ** the request from the other client (created by the clone)). As a
  ** workaround for that we call memcached_quit to send the quit command
  ** to the server and wait for the response ;-) If you use the test code
  ** as an example for your own code, please note that you shouldn't need
  ** to do this ;-)
  */
  memcached_quit(memc);

  /*
  ** "bubba" should now be stored on all of our servers. We don't have an
  ** easy to use API to address each individual server, so I'll just iterate
  ** through a bunch of "master keys" and I should most likely hit all of the
  ** servers...
  */
  for (int x= 'a'; x <= 'z'; ++x)
  {
    const char key[2]= { (char)x, 0 };
    size_t len;
    uint32_t flags;
    char *val= memcached_get_by_key(memc_clone, key, 1, "bubba", 5,
                                    &len, &flags, &rc);
    test_true(rc == MEMCACHED_SUCCESS);
    test_true(val != NULL);
    free(val);
  }

  memcached_free(memc_clone);

  return TEST_SUCCESS;
}

test_return_t replication_get_test(memcached_st *memc)
{
  memcached_return_t rc;

  /*
   * Don't do the following in your code. I am abusing the internal details
   * within the library, and this is not a supported interface.
   * This is to verify correct behavior in the library
   */
  for (uint32_t host= 0; host < memcached_server_count(memc); ++host)
  {
    memcached_st *memc_clone= memcached_clone(NULL, memc);
    memcached_server_instance_st instance=
      memcached_server_instance_by_position(memc_clone, host);

    ((memcached_server_write_instance_st)instance)->port= 0;

    for (int x= 'a'; x <= 'z'; ++x)
    {
      const char key[2]= { (char)x, 0 };
      size_t len;
      uint32_t flags;
      char *val= memcached_get_by_key(memc_clone, key, 1, "bubba", 5,
                                      &len, &flags, &rc);
      test_true(rc == MEMCACHED_SUCCESS);
      test_true(val != NULL);
      free(val);
    }

    memcached_free(memc_clone);
  }

  return TEST_SUCCESS;
}

test_return_t replication_mget_test(memcached_st *memc)
{
  memcached_return_t rc;
  memcached_st *memc_clone= memcached_clone(NULL, memc);
  memcached_behavior_set(memc_clone, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 0);

  const char *keys[]= { "bubba", "key1", "key2", "key3" };
  size_t len[]= { 5, 4, 4, 4 };

  for (size_t x= 0; x< 4; ++x)
  {
    rc= memcached_set(memc, keys[x], len[x], "0", 1, 0, 0);
    test_true(rc == MEMCACHED_SUCCESS);
  }

  /*
  ** We are using the quiet commands to store the replicas, so we need
  ** to ensure that all of them are processed before we can continue.
  ** In the test we go directly from storing the object to trying to
  ** receive the object from all of the different servers, so we
  ** could end up in a race condition (the memcached server hasn't yet
  ** processed the quiet command from the replication set when it process
  ** the request from the other client (created by the clone)). As a
  ** workaround for that we call memcached_quit to send the quit command
  ** to the server and wait for the response ;-) If you use the test code
  ** as an example for your own code, please note that you shouldn't need
  ** to do this ;-)
  */
  memcached_quit(memc);

  /*
   * Don't do the following in your code. I am abusing the internal details
   * within the library, and this is not a supported interface.
   * This is to verify correct behavior in the library
   */
  memcached_result_st result_obj;
  for (uint32_t host= 0; host < memc_clone->number_of_hosts; host++)
  {
    memcached_st *new_clone= memcached_clone(NULL, memc);
    memcached_server_instance_st instance=
      memcached_server_instance_by_position(new_clone, host);
    ((memcached_server_write_instance_st)instance)->port= 0;

    for (int x= 'a'; x <= 'z'; ++x)
    {
      char key[2]= { (char)x, 0 };

      rc= memcached_mget_by_key(new_clone, key, 1, keys, len, 4);
      test_true(rc == MEMCACHED_SUCCESS);

      memcached_result_st *results= memcached_result_create(new_clone, &result_obj);
      test_true(results);

      int hits= 0;
      while ((results= memcached_fetch_result(new_clone, &result_obj, &rc)) != NULL)
      {
        hits++;
      }
      test_true(hits == 4);
      memcached_result_free(&result_obj);
    }

    memcached_free(new_clone);
  }

  memcached_free(memc_clone);

  return TEST_SUCCESS;
}

test_return_t replication_randomize_mget_test(memcached_st *memc)
{
  memcached_result_st result_obj;
  memcached_return_t rc;
  memcached_st *memc_clone= memcached_clone(NULL, memc);
  memcached_behavior_set(memc_clone, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 3);
  memcached_behavior_set(memc_clone, MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ, 1);

  const char *keys[]= { "key1", "key2", "key3", "key4", "key5", "key6", "key7" };
  size_t len[]= { 4, 4, 4, 4, 4, 4, 4 };

  for (size_t x= 0; x< 7; ++x)
  {
    rc= memcached_set(memc, keys[x], len[x], "1", 1, 0, 0);
    test_true(rc == MEMCACHED_SUCCESS);
  }

  memcached_quit(memc);

  for (size_t x= 0; x< 7; ++x)
  {
    const char key[2]= { (char)x, 0 };

    rc= memcached_mget_by_key(memc_clone, key, 1, keys, len, 7);
    test_true(rc == MEMCACHED_SUCCESS);

    memcached_result_st *results= memcached_result_create(memc_clone, &result_obj);
    test_true(results);

    int hits= 0;
    while ((results= memcached_fetch_result(memc_clone, &result_obj, &rc)) != NULL)
    {
      ++hits;
    }
    test_true(hits == 7);
    memcached_result_free(&result_obj);
  }
  memcached_free(memc_clone);
  return TEST_SUCCESS;
}

test_return_t replication_delete_test(memcached_st *memc)
{
  memcached_return_t rc;
  memcached_st *memc_clone= memcached_clone(NULL, memc);
  /* Delete the items from all of the servers except 1 */
  uint64_t repl= memcached_behavior_get(memc,
                                        MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, --repl);

  const char *keys[]= { "bubba", "key1", "key2", "key3" };
  size_t len[]= { 5, 4, 4, 4 };

  for (size_t x= 0; x< 4; ++x)
  {
    rc= memcached_delete_by_key(memc, keys[0], len[0], keys[x], len[x], 0);
    test_true(rc == MEMCACHED_SUCCESS);
  }

  /*
   * Don't do the following in your code. I am abusing the internal details
   * within the library, and this is not a supported interface.
   * This is to verify correct behavior in the library
   */
  uint32_t hash= memcached_generate_hash(memc, keys[0], len[0]);
  for (uint32_t x= 0; x < (repl + 1); ++x)
  {
    memcached_server_instance_st instance=
      memcached_server_instance_by_position(memc_clone, x);

    ((memcached_server_write_instance_st)instance)->port= 0;
    if (++hash == memc_clone->number_of_hosts)
      hash= 0;
  }

  memcached_result_st result_obj;
  for (uint32_t host= 0; host < memc_clone->number_of_hosts; ++host)
  {
    for (size_t x= 'a'; x <= 'z'; ++x)
    {
      const char key[2]= { (char)x, 0 };

      rc= memcached_mget_by_key(memc_clone, key, 1, keys, len, 4);
      test_true(rc == MEMCACHED_SUCCESS);

      memcached_result_st *results= memcached_result_create(memc_clone, &result_obj);
      test_true(results);

      int hits= 0;
      while ((results= memcached_fetch_result(memc_clone, &result_obj, &rc)) != NULL)
      {
        ++hits;
      }
      test_true(hits == 4);
      memcached_result_free(&result_obj);
    }
  }
  memcached_free(memc_clone);

  return TEST_SUCCESS;
}
