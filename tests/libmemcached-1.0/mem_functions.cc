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

#include "mem_config.h"
#include "libtest/test.hpp"

#if defined(HAVE_LIBUUID) && HAVE_LIBUUID
# include <uuid/uuid.h>
#endif

/*
  Test cases
*/

#include "libmemcached-1.0/memcached.h"
#include "libmemcached/is.h"
#include "libmemcached/server_instance.h"

#include "libhashkit-1.0/hashkit.h"

#include "libtest/memcached.hpp"

#include <cerrno>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "libtest/server.h"

#include "bin/generator.h"

#define SMALL_STRING_LEN 1024

#include "libtest/test.hpp"

using namespace libtest;

#include "libmemcachedutil-1.0/util.h"

#include "tests/hash_results.h"

#include "tests/libmemcached-1.0/callback_counter.h"
#include "tests/libmemcached-1.0/fetch_all_results.h"
#include "tests/libmemcached-1.0/mem_functions.h"
#include "tests/libmemcached-1.0/setup_and_teardowns.h"
#include "tests/print.h"
#include "tests/debug.h"
#include "tests/memc.hpp"

#define UUID_STRING_MAXLENGTH 36

#include "tests/keys.hpp"

#include "libmemcached/instance.hpp"


test_return_t mget_end(memcached_st *memc)
{
  const char *keys[]= { "foo", "foo2" };
  size_t lengths[]= { 3, 4 };
  const char *values[]= { "fjord", "41" };

  // Set foo and foo2
  for (size_t x= 0; x < test_array_length(keys); x++)
  {
    test_compare(MEMCACHED_SUCCESS,
                 memcached_set(memc,
                               keys[x], lengths[x],
                               values[x], strlen(values[x]),
                               time_t(0), uint32_t(0)));
  }

  char *string;
  size_t string_length;
  uint32_t flags;

  // retrieve both via mget
  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc,
                              keys, lengths,
                              test_array_length(keys)));

  char key[MEMCACHED_MAX_KEY];
  size_t key_length;
  memcached_return_t rc;

  // this should get both
  for (size_t x= 0; x < test_array_length(keys); x++)
  {
    string= memcached_fetch(memc, key, &key_length, &string_length,
                            &flags, &rc);
    test_compare(MEMCACHED_SUCCESS, rc);
    int val = 0;
    if (key_length == 4)
    {
      val= 1;
    }

    test_compare(string_length, strlen(values[val]));
    test_true(strncmp(values[val], string, string_length) == 0);
    free(string);
  }

  // this should indicate end
  string= memcached_fetch(memc, key, &key_length, &string_length, &flags, &rc);
  test_compare(MEMCACHED_END, rc);
  test_null(string);

  // now get just one
  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, lengths, 1));

  string= memcached_fetch(memc, key, &key_length, &string_length, &flags, &rc);
  test_compare(key_length, lengths[0]);
  test_true(strncmp(keys[0], key, key_length) == 0);
  test_compare(string_length, strlen(values[0]));
  test_true(strncmp(values[0], string, string_length) == 0);
  test_compare(MEMCACHED_SUCCESS, rc);
  free(string);

  // this should indicate end
  string= memcached_fetch(memc, key, &key_length, &string_length, &flags, &rc);
  test_compare(MEMCACHED_END, rc);
  test_null(string);

  return TEST_SUCCESS;
}

test_return_t mget_result_test(memcached_st *memc)
{
  const char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};

  memcached_result_st results_obj;
  memcached_result_st *results= memcached_result_create(memc, &results_obj);
  test_true(results);
  test_true(&results_obj == results);

  /* We need to empty the server before continueing test */
  test_compare(MEMCACHED_SUCCESS,
               memcached_flush(memc, 0));

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  memcached_return_t rc;
  while ((results= memcached_fetch_result(memc, &results_obj, &rc)))
  {
    test_true(results);
  }

  while ((results= memcached_fetch_result(memc, &results_obj, &rc))) { test_true(false); /* We should never see a value returned */ };
  test_false(results);
  test_compare(MEMCACHED_NOTFOUND, rc);

  for (uint32_t x= 0; x < 3; x++)
  {
    rc= memcached_set(memc, keys[x], key_length[x],
                      keys[x], key_length[x],
                      (time_t)50, (uint32_t)9);
    test_true(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED);
  }

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  while ((results= memcached_fetch_result(memc, &results_obj, &rc)))
  {
    test_true(results);
    test_true(&results_obj == results);
    test_compare(MEMCACHED_SUCCESS, rc);
    test_memcmp(memcached_result_key_value(results),
                memcached_result_value(results),
                memcached_result_length(results));
    test_compare(memcached_result_key_length(results), memcached_result_length(results));
  }

  memcached_result_free(&results_obj);

  return TEST_SUCCESS;
}

test_return_t mget_result_alloc_test(memcached_st *memc)
{
  const char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};

  memcached_result_st *results;

  /* We need to empty the server before continueing test */
  test_compare(MEMCACHED_SUCCESS,
               memcached_flush(memc, 0));

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  memcached_return_t rc;
  while ((results= memcached_fetch_result(memc, NULL, &rc)))
  {
    test_true(results);
  }
  test_false(results);
  test_compare(MEMCACHED_NOTFOUND, rc);

  for (uint32_t x= 0; x < 3; x++)
  {
    rc= memcached_set(memc, keys[x], key_length[x],
                      keys[x], key_length[x],
                      (time_t)50, (uint32_t)9);
    test_true(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED);
  }

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  uint32_t x= 0;
  while ((results= memcached_fetch_result(memc, NULL, &rc)))
  {
    test_true(results);
    test_compare(MEMCACHED_SUCCESS, rc);
    test_compare(memcached_result_key_length(results), memcached_result_length(results));
    test_memcmp(memcached_result_key_value(results),
                memcached_result_value(results),
                memcached_result_length(results));
    memcached_result_free(results);
    x++;
  }

  return TEST_SUCCESS;
}

test_return_t mget_result_function(memcached_st *memc)
{
  const char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};
  size_t counter;
  memcached_execute_fn callbacks[1];

  for (uint32_t x= 0; x < 3; x++)
  {
    test_compare(return_value_based_on_buffering(memc),
                 memcached_set(memc, keys[x], key_length[x],
                               keys[x], key_length[x],
                               time_t(50), uint32_t(9)));
  }
  test_compare(MEMCACHED_SUCCESS, memcached_flush_buffers(memc));
  memcached_quit(memc);

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  callbacks[0]= &callback_counter;
  counter= 0;

  test_compare(MEMCACHED_SUCCESS,
               memcached_fetch_execute(memc, callbacks, (void *)&counter, 1));

  test_compare(size_t(3), counter);

  return TEST_SUCCESS;
}

test_return_t mget_test(memcached_st *memc)
{
  const char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};

  char return_key[MEMCACHED_MAX_KEY];
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  uint32_t flags;
  memcached_return_t rc;
  while ((return_value= memcached_fetch(memc, return_key, &return_key_length,
                                        &return_value_length, &flags, &rc)))
  {
    test_true(return_value);
  }
  test_false(return_value);
  test_zero(return_value_length);
  test_compare(MEMCACHED_NOTFOUND, rc);

  for (uint32_t x= 0; x < 3; x++)
  {
    rc= memcached_set(memc, keys[x], key_length[x],
                      keys[x], key_length[x],
                      (time_t)50, (uint32_t)9);
    test_true(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED);
  }
  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 3));

  uint32_t x= 0;
  while ((return_value= memcached_fetch(memc, return_key, &return_key_length,
                                        &return_value_length, &flags, &rc)))
  {
    test_true(return_value);
    test_compare(MEMCACHED_SUCCESS, rc);
    if (not memc->_namespace)
    {
      test_compare(return_key_length, return_value_length);
      test_memcmp(return_value, return_key, return_value_length);
    }
    free(return_value);
    x++;
  }

  return TEST_SUCCESS;
}

test_return_t mget_execute(memcached_st *original_memc)
{
  test_skip(true, memcached_behavior_get(original_memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL));

  memcached_st *memc= create_single_instance_memcached(original_memc, "--BINARY-PROTOCOL");
  test_true(memc);

  keys_st keys(20480);

  /* First add all of the items.. */
  char blob[1024] = {0};

  for (size_t x= 0; x < keys.size(); ++x)
  {
    uint64_t query_id= memcached_query_id(memc);
    memcached_return_t rc= memcached_add(memc,
                                         keys.key_at(x), keys.length_at(x),
                                         blob, sizeof(blob),
                                         0, 0);
    ASSERT_TRUE_(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED, "Returned %s", memcached_strerror(NULL, rc));
    test_compare(query_id +1, memcached_query_id(memc));
  }

  /* Try to get all of them with a large multiget */
  size_t counter= 0;
  memcached_execute_fn callbacks[]= { &callback_counter };
  test_compare(MEMCACHED_SUCCESS,
               memcached_mget_execute(memc,
                                      keys.keys_ptr(), keys.lengths_ptr(),
                                      keys.size(), callbacks, &counter, 1));

  {
    uint64_t query_id= memcached_query_id(memc);
    test_compare(MEMCACHED_SUCCESS,
                 memcached_fetch_execute(memc, callbacks, (void *)&counter, 1));
    test_compare(query_id, memcached_query_id(memc));

    /* Verify that we got all of the items */
    test_compare(keys.size(), counter);
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}



test_return_t memcached_fetch_result_NOT_FOUND(memcached_st *memc)
{
  memcached_return_t rc;

  const char *key= "not_found";
  size_t key_length= test_literal_param_size("not_found");

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, &key, &key_length, 1));

  memcached_result_st *result= memcached_fetch_result(memc, NULL, &rc);
  test_null(result);
  test_compare(MEMCACHED_NOTFOUND, rc);

  memcached_result_free(result);

  return TEST_SUCCESS;
}

/*
  Bug found where incr was not returning MEMCACHED_NOTFOUND when object did not exist.
*/
test_return_t user_supplied_bug12(memcached_st *memc)
{
  memcached_return_t rc;
  uint32_t flags;
  size_t value_length;
  char *value;
  uint64_t number_value;

  value= memcached_get(memc, "autoincrement", strlen("autoincrement"),
                       &value_length, &flags, &rc);
  test_null(value);
  test_compare(MEMCACHED_NOTFOUND, rc);

  rc= memcached_increment(memc, "autoincrement", strlen("autoincrement"),
                          1, &number_value);
  test_null(value);
  /* The binary protocol will set the key if it doesn't exist */
  if (memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL) == 1)
  {
    test_compare(MEMCACHED_SUCCESS, rc);
  }
  else
  {
    test_compare(MEMCACHED_NOTFOUND, rc);
  }

  test_compare(MEMCACHED_SUCCESS,
               memcached_set(memc, "autoincrement", strlen("autoincrement"), "1", 1, 0, 0));

  value= memcached_get(memc, "autoincrement", strlen("autoincrement"), &value_length, &flags, &rc);
  test_true(value);
  free(value);

  test_compare(MEMCACHED_SUCCESS,
               memcached_increment(memc, "autoincrement", strlen("autoincrement"), 1, &number_value));
  test_compare(2UL, number_value);

  return TEST_SUCCESS;
}

/*
  Bug found where command total one more than MEMCACHED_MAX_BUFFER
  set key34567890 0 0 8169 \r\n is sent followed by buffer of size 8169, followed by 8169
*/
test_return_t user_supplied_bug13(memcached_st *memc)
{
  char key[] = "key34567890";

  char commandFirst[]= "set key34567890 0 0 ";
  char commandLast[] = " \r\n"; /* first line of command sent to server */
  size_t commandLength;

  commandLength = strlen(commandFirst) + strlen(commandLast) + 4; /* 4 is number of characters in size, probably 8196 */

  size_t overflowSize = MEMCACHED_MAX_BUFFER - commandLength;

  for (size_t testSize= overflowSize - 1; testSize < overflowSize + 1; testSize++)
  {
    char *overflow= new (std::nothrow) char[testSize];
    test_true(overflow);

    memset(overflow, 'x', testSize);
    test_compare(MEMCACHED_SUCCESS,
                 memcached_set(memc, key, strlen(key),
                               overflow, testSize, 0, 0));
    delete [] overflow;
  }

  return TEST_SUCCESS;
}


/*
  Test values of many different sizes
  Bug found where command total one more than MEMCACHED_MAX_BUFFER
  set key34567890 0 0 8169 \r\n
  is sent followed by buffer of size 8169, followed by 8169
*/
test_return_t user_supplied_bug14(memcached_st *memc)
{
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, true);

  libtest::vchar_t value;
  value.reserve(18000);
  for (ptrdiff_t x= 0; x < 18000; x++)
  {
    value.push_back((char) (x % 127));
  }

  for (size_t current_length= 1; current_length < value.size(); current_length++)
  {
    memcached_return_t rc= memcached_set(memc, test_literal_param("foo"),
                                         &value[0], current_length,
                                         (time_t)0, (uint32_t)0);
    ASSERT_TRUE_(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED, "Instead got %s", memcached_strerror(NULL, rc));

    size_t string_length;
    uint32_t flags;
    char *string= memcached_get(memc, test_literal_param("foo"),
                                &string_length, &flags, &rc);

    test_compare(MEMCACHED_SUCCESS, rc);
    test_compare(string_length, current_length);
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%u", uint32_t(string_length));
    test_memcmp(string, &value[0], string_length);

    free(string);
  }

  return TEST_SUCCESS;
}

/*
  From Andrei on IRC
*/

test_return_t user_supplied_bug19(memcached_st *)
{
  memcached_return_t res;

  memcached_st *memc= memcached(test_literal_param("--server=localhost:11311/?100 --server=localhost:11312/?100"));

  const memcached_instance_st * server= memcached_server_by_key(memc, "a", 1, &res);
  test_true(server);

  memcached_free(memc);

  return TEST_SUCCESS;
}

/* CAS test from Andei */
test_return_t user_supplied_bug20(memcached_st *memc)
{
  const char *key= "abc";
  size_t key_len= strlen("abc");

  test_skip(MEMCACHED_SUCCESS, memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SUPPORT_CAS, true));

  test_compare(MEMCACHED_SUCCESS,
               memcached_set(memc,
                             test_literal_param("abc"),
                             test_literal_param("foobar"),
                             (time_t)0, (uint32_t)0));

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, &key, &key_len, 1));

  memcached_result_st result_obj;
  memcached_result_st *result= memcached_result_create(memc, &result_obj);
  test_true(result);

  memcached_result_create(memc, &result_obj);
  memcached_return_t status;
  result= memcached_fetch_result(memc, &result_obj, &status);

  test_true(result);
  test_compare(MEMCACHED_SUCCESS, status);

  memcached_result_free(result);

  return TEST_SUCCESS;
}

/* Large mget() of missing keys with binary proto
 *
 * If many binary quiet commands (such as getq's in an mget) fill the output
 * buffer and the server chooses not to respond, memcached_flush hangs. See
 * http://lists.tangent.org/pipermail/libmemcached/2009-August/000918.html
 */

/* sighandler_t function that always asserts false */
static __attribute__((noreturn)) void fail(int)
{
  fatal_assert(0);
}


test_return_t _user_supplied_bug21(memcached_st* memc, size_t key_count)
{
#ifdef WIN32
  (void)memc;
  (void)key_count;
  return TEST_SKIPPED;
#else
  void (*oldalarm)(int);

  memcached_st *memc_clone= memcached_clone(NULL, memc);
  test_true(memc_clone);

  /* only binproto uses getq for mget */
  test_compare(MEMCACHED_SUCCESS, memcached_behavior_set(memc_clone, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, true));

  /* empty the cache to ensure misses (hence non-responses) */
  test_compare(MEMCACHED_SUCCESS, memcached_flush(memc_clone, 0));

  keys_st keys(key_count);

  oldalarm= signal(SIGALRM, fail);
  alarm(5);

  test_compare_got(MEMCACHED_SUCCESS,
                   memcached_mget(memc_clone, keys.keys_ptr(), keys.lengths_ptr(), keys.size()),
                   memcached_last_error_message(memc_clone));

  alarm(0);
  signal(SIGALRM, oldalarm);

  memcached_return_t rc;
  uint32_t flags;
  char return_key[MEMCACHED_MAX_KEY];
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;
  while ((return_value= memcached_fetch(memc, return_key, &return_key_length,
                                        &return_value_length, &flags, &rc)))
  {
    test_false(return_value); // There are no keys to fetch, so the value should never be returned
  }
  test_compare(MEMCACHED_NOTFOUND, rc);
  test_zero(return_value_length);
  test_zero(return_key_length);
  test_false(return_key[0]);
  test_false(return_value);

  memcached_free(memc_clone);

  return TEST_SUCCESS;
#endif
}

test_return_t user_supplied_bug21(memcached_st *memc)
{
  test_skip(TEST_SUCCESS, pre_binary(memc));

  /* should work as of r580 */
  test_compare(TEST_SUCCESS,
               _user_supplied_bug21(memc, 10));

  /* should fail as of r580 */
  test_compare(TEST_SUCCESS,
               _user_supplied_bug21(memc, 1000));

  return TEST_SUCCESS;
}

test_return_t comparison_operator_memcached_st_and__memcached_return_t_TEST(memcached_st *)
{
  test::Memc memc_;

  memcached_st *memc= &memc_;

  ASSERT_EQ(memc, MEMCACHED_SUCCESS);
  test_compare(memc, MEMCACHED_SUCCESS);

  ASSERT_NEQ(memc, MEMCACHED_FAILURE);

  return TEST_SUCCESS;
}


static void my_free(const memcached_st *ptr, void *mem, void *context)
{
  (void)context;
  (void)ptr;
#ifdef HARD_MALLOC_TESTS
  void *real_ptr= (mem == NULL) ? mem : (void*)((caddr_t)mem - 8);
  free(real_ptr);
#else
  free(mem);
#endif
}


static void *my_malloc(const memcached_st *ptr, const size_t size, void *context)
{
  (void)context;
  (void)ptr;
#ifdef HARD_MALLOC_TESTS
  void *ret= malloc(size + 8);
  if (ret != NULL)
  {
    ret= (void*)((caddr_t)ret + 8);
  }
#else
  void *ret= malloc(size);
#endif

  if (ret != NULL)
  {
    memset(ret, 0xff, size);
  }

  return ret;
}


static void *my_realloc(const memcached_st *ptr, void *mem, const size_t size, void *)
{
#ifdef HARD_MALLOC_TESTS
  void *real_ptr= (mem == NULL) ? NULL : (void*)((caddr_t)mem - 8);
  void *nmem= realloc(real_ptr, size + 8);

  void *ret= NULL;
  if (nmem != NULL)
  {
    ret= (void*)((caddr_t)nmem + 8);
  }

  return ret;
#else
  (void)ptr;
  return realloc(mem, size);
#endif
}


static void *my_calloc(const memcached_st *ptr, size_t nelem, const size_t size, void *)
{
#ifdef HARD_MALLOC_TESTS
  void *mem= my_malloc(ptr, nelem * size);
  if (mem)
  {
    memset(mem, 0, nelem * size);
  }

  return mem;
#else
  (void)ptr;
  return calloc(nelem, size);
#endif
}

#ifdef MEMCACHED_ENABLE_DEPRECATED
test_return_t deprecated_set_memory_alloc(memcached_st *memc)
{
  void *test_ptr= NULL;
  void *cb_ptr= NULL;
  {
    memcached_malloc_fn malloc_cb= (memcached_malloc_fn)my_malloc;
    cb_ptr= *(void **)&malloc_cb;
    memcached_return_t rc;

    test_compare(MEMCACHED_SUCCESS,
                 memcached_callback_set(memc, MEMCACHED_CALLBACK_MALLOC_FUNCTION, cb_ptr));
    test_ptr= memcached_callback_get(memc, MEMCACHED_CALLBACK_MALLOC_FUNCTION, &rc);
    test_compare(MEMCACHED_SUCCESS, rc);
    test_true(test_ptr == cb_ptr);
  }

  {
    memcached_realloc_fn realloc_cb=
      (memcached_realloc_fn)my_realloc;
    cb_ptr= *(void **)&realloc_cb;
    memcached_return_t rc;

    test_compare(MEMCACHED_SUCCESS,
                 memcached_callback_set(memc, MEMCACHED_CALLBACK_REALLOC_FUNCTION, cb_ptr));
    test_ptr= memcached_callback_get(memc, MEMCACHED_CALLBACK_REALLOC_FUNCTION, &rc);
    test_compare(MEMCACHED_SUCCESS, rc);
    test_true(test_ptr == cb_ptr);
  }

  {
    memcached_free_fn free_cb=
      (memcached_free_fn)my_free;
    cb_ptr= *(void **)&free_cb;
    memcached_return_t rc;

    test_compare(MEMCACHED_SUCCESS,
                 memcached_callback_set(memc, MEMCACHED_CALLBACK_FREE_FUNCTION, cb_ptr));
    test_ptr= memcached_callback_get(memc, MEMCACHED_CALLBACK_FREE_FUNCTION, &rc);
    test_compare(MEMCACHED_SUCCESS, rc);
    test_true(test_ptr == cb_ptr);
  }

  return TEST_SUCCESS;
}
#endif


test_return_t set_memory_alloc(memcached_st *memc)
{
  test_compare(MEMCACHED_INVALID_ARGUMENTS,
               memcached_set_memory_allocators(memc, NULL, my_free,
                                               my_realloc, my_calloc, NULL));

  test_compare(MEMCACHED_SUCCESS,
               memcached_set_memory_allocators(memc, my_malloc, my_free,
                                               my_realloc, my_calloc, NULL));

  memcached_malloc_fn mem_malloc;
  memcached_free_fn mem_free;
  memcached_realloc_fn mem_realloc;
  memcached_calloc_fn mem_calloc;
  memcached_get_memory_allocators(memc, &mem_malloc, &mem_free,
                                  &mem_realloc, &mem_calloc);

  test_true(mem_malloc == my_malloc);
  test_true(mem_realloc == my_realloc);
  test_true(mem_calloc == my_calloc);
  test_true(mem_free == my_free);

  return TEST_SUCCESS;
}



/*
  Test case adapted from John Gorman <johngorman2@gmail.com>

  We are testing the error condition when we connect to a server via memcached_get()
  but find that the server is not available.
*/
test_return_t memcached_get_MEMCACHED_ERRNO(memcached_st *)
{
  size_t len;
  uint32_t flags;
  memcached_return rc;

  // Create a handle.
  memcached_st *tl_memc_h= memcached(test_literal_param("--server=localhost:9898 --server=localhost:9899")); // This server should not exist

  // See if memcached is reachable.
  char *value= memcached_get(tl_memc_h,
                             test_literal_param(__func__),
                             &len, &flags, &rc);

  test_false(value);
  test_zero(len);
  test_true(memcached_failed(rc));

  memcached_free(tl_memc_h);

  return TEST_SUCCESS;
}

/*
  Test case adapted from John Gorman <johngorman2@gmail.com>

  We are testing the error condition when we connect to a server via memcached_get_by_key()
  but find that the server is not available.
*/
test_return_t memcached_get_by_key_MEMCACHED_ERRNO(memcached_st *)
{
  size_t len;
  uint32_t flags;
  memcached_return rc;

  // Create a handle.
  memcached_st *tl_memc_h= memcached_create(NULL);
  memcached_server_st *servers= memcached_servers_parse("localhost:9898,localhost:9899"); // This server should not exist
  memcached_server_push(tl_memc_h, servers);
  memcached_server_list_free(servers);

  // See if memcached is reachable.
  char *value= memcached_get_by_key(tl_memc_h,
                                    test_literal_param(__func__), // Key
                                    test_literal_param(__func__), // Value
                                    &len, &flags, &rc);

  test_false(value);
  test_zero(len);
  test_true(memcached_failed(rc));

  memcached_free(tl_memc_h);

  return TEST_SUCCESS;
}

/* Test memcached_server_get_last_disconnect
 * For a working server set, shall be NULL
 * For a set of non existing server, shall not be NULL
 */
test_return_t test_get_last_disconnect(memcached_st *memc)
{
  memcached_return_t rc;
  const memcached_instance_st * disconnected_server;

  /* With the working set of server */
  const char *key= "marmotte";
  const char *value= "milka";

  memcached_reset_last_disconnected_server(memc);
  test_false(memc->last_disconnected_server);
  rc= memcached_set(memc, key, strlen(key),
                    value, strlen(value),
                    (time_t)0, (uint32_t)0);
  test_true(rc == MEMCACHED_SUCCESS || rc == MEMCACHED_BUFFERED);

  disconnected_server = memcached_server_get_last_disconnect(memc);
  test_false(disconnected_server);

  /* With a non existing server */
  memcached_st *mine;
  memcached_server_st *servers;

  const char *server_list= "localhost:9";

  servers= memcached_servers_parse(server_list);
  test_true(servers);
  mine= memcached_create(NULL);
  rc= memcached_server_push(mine, servers);
  test_compare(MEMCACHED_SUCCESS, rc);
  memcached_server_list_free(servers);
  test_true(mine);

  rc= memcached_set(mine, key, strlen(key),
                    value, strlen(value),
                    (time_t)0, (uint32_t)0);
  test_true(memcached_failed(rc));

  disconnected_server= memcached_server_get_last_disconnect(mine);
  test_true_got(disconnected_server, memcached_strerror(mine, rc));
  test_compare(in_port_t(9), memcached_server_port(disconnected_server));
  test_false(strncmp(memcached_server_name(disconnected_server),"localhost",9));

  memcached_quit(mine);
  memcached_free(mine);

  return TEST_SUCCESS;
}

test_return_t test_multiple_get_last_disconnect(memcached_st *)
{
  const char *server_string= "--server=localhost:8888 --server=localhost:8889 --server=localhost:8890 --server=localhost:8891 --server=localhost:8892";
  char buffer[BUFSIZ];

  test_compare(MEMCACHED_SUCCESS,
               libmemcached_check_configuration(server_string, strlen(server_string), buffer, sizeof(buffer)));

  memcached_st *memc= memcached(server_string, strlen(server_string));
  test_true(memc);

  // We will just use the error strings as our keys
  uint32_t counter= 100;
  while (--counter)
  {
    for (int x= int(MEMCACHED_SUCCESS); x < int(MEMCACHED_MAXIMUM_RETURN); ++x)
    {
      const char *msg=  memcached_strerror(memc, memcached_return_t(x));
      memcached_return_t ret= memcached_set(memc, msg, strlen(msg), NULL, 0, (time_t)0, (uint32_t)0);
      test_true_got((ret == MEMCACHED_CONNECTION_FAILURE or ret == MEMCACHED_SERVER_TEMPORARILY_DISABLED), memcached_last_error_message(memc));

      const memcached_instance_st * disconnected_server= memcached_server_get_last_disconnect(memc);
      test_true(disconnected_server);
      test_strcmp("localhost", memcached_server_name(disconnected_server));
      test_true(memcached_server_port(disconnected_server) >= 8888 and memcached_server_port(disconnected_server) <= 8892);

      if (random() % 2)
      {
        memcached_reset_last_disconnected_server(memc);
      }
    }
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

/*
 * This test ensures that the failure counter isn't incremented during
 * normal termination of the memcached instance.
 */
test_return_t wrong_failure_counter_test(memcached_st *original_memc)
{
  memcached_st* memc= create_single_instance_memcached(original_memc, NULL);

  /* Ensure that we are connected to the server by setting a value */
  memcached_return_t rc= memcached_set(memc,
                                       test_literal_param(__func__), // Key
                                       test_literal_param(__func__), // Value
                                       time_t(0), uint32_t(0));
  test_true(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED);


  const memcached_instance_st * instance= memcached_server_instance_by_position(memc, 0);

  /* The test is to see that the memcached_quit doesn't increase the
   * the server failure conter, so let's ensure that it is zero
   * before sending quit
 */
  ((memcached_server_write_instance_st)instance)->server_failure_counter= 0;

  memcached_quit(memc);

  /* Verify that it memcached_quit didn't increment the failure counter
   * Please note that this isn't bullet proof, because an error could
   * occur...
 */
  test_zero(instance->server_failure_counter);

  memcached_free(memc);

  return TEST_SUCCESS;
}

/*
 * This tests ensures expected disconnections (for some behavior changes
 * for instance) do not wrongly increase failure counter
 */
test_return_t wrong_failure_counter_two_test(memcached_st *memc)
{
  /* Set value to force connection to the server */
  const char *key= "marmotte";
  const char *value= "milka";

  test_compare_hint(MEMCACHED_SUCCESS,
                    memcached_set(memc, key, strlen(key),
                                  value, strlen(value),
                                  (time_t)0, (uint32_t)0),
                    memcached_last_error_message(memc));


  /* put failure limit to 1 */
  test_compare(MEMCACHED_SUCCESS,
               memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, true));

  /* Put a retry timeout to effectively activate failure_limit effect */
  test_compare(MEMCACHED_SUCCESS,
               memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 1));

  /* change behavior that triggers memcached_quit()*/
  test_compare(MEMCACHED_SUCCESS,
               memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, true));


  /* Check if we still are connected */
  uint32_t flags;
  size_t string_length;
  memcached_return rc;
  char *string= memcached_get(memc, key, strlen(key),
                              &string_length, &flags, &rc);

  test_compare_got(MEMCACHED_SUCCESS, rc, memcached_strerror(NULL, rc));
  test_true(string);
  free(string);

  return TEST_SUCCESS;
}


#define regression_bug_655423_COUNT 6000
test_return_t regression_bug_655423(memcached_st *memc)
{
  memcached_st *clone= memcached_clone(NULL, memc);
  memc= NULL; // Just to make sure it is not used
  test_true(clone);
  char payload[100];

#ifdef __APPLE__
  return TEST_SKIPPED;
#endif

  test_skip(MEMCACHED_SUCCESS, memcached_behavior_set(clone, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1));
  test_skip(MEMCACHED_SUCCESS, memcached_behavior_set(clone, MEMCACHED_BEHAVIOR_SUPPORT_CAS, 1));
  test_skip(MEMCACHED_SUCCESS, memcached_behavior_set(clone, MEMCACHED_BEHAVIOR_TCP_NODELAY, 1));
  test_skip(MEMCACHED_SUCCESS, memcached_behavior_set(clone, MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH, 1));

  memset(payload, int('x'), sizeof(payload));

  keys_st keys(regression_bug_655423_COUNT);

  for (size_t x= 0; x < keys.size(); x++)
  {
    test_compare(MEMCACHED_SUCCESS, memcached_set(clone,
                                                  keys.key_at(x),
                                                  keys.length_at(x),
                                                  payload, sizeof(payload), 0, 0));
  }

  for (size_t x= 0; x < keys.size(); x++)
  {
    size_t value_length;
    memcached_return_t rc;
    char *value= memcached_get(clone,
                               keys.key_at(x),
                               keys.length_at(x),
                               &value_length, NULL, &rc);

    if (rc == MEMCACHED_NOTFOUND)
    {
      test_false(value);
      test_zero(value_length);
      continue;
    }

    test_compare(MEMCACHED_SUCCESS, rc);
    test_true(value);
    test_compare(100LLU, value_length);
    free(value);
  }

  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(clone,
                              keys.keys_ptr(), keys.lengths_ptr(),
                              keys.size()));

  uint32_t count= 0;
  memcached_result_st *result= NULL;
  while ((result= memcached_fetch_result(clone, result, NULL)))
  {
    test_compare(size_t(100), memcached_result_length(result));
    count++;
  }

  test_true(count > 100); // If we don't get back atleast this, something is up

  memcached_free(clone);

  return TEST_SUCCESS;
}

/*
 * Test that ensures that buffered set to not trigger problems during io_flush
 */
#define regression_bug_490520_COUNT 200480
test_return_t regression_bug_490520(memcached_st *original_memc)
{
  memcached_st* memc= create_single_instance_memcached(original_memc, NULL);

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK,1);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS,1);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 1000);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT,1);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 3600);

  /* First add all of the items.. */
  char blob[3333] = {0};
  for (uint32_t x= 0; x < regression_bug_490520_COUNT; ++x)
  {
    char key[251];
    int key_length= snprintf(key, sizeof(key), "0200%u", x);

    memcached_return rc= memcached_set(memc, key, key_length, blob, sizeof(blob), 0, 0);
    test_true_got(rc == MEMCACHED_SUCCESS or rc == MEMCACHED_BUFFERED, memcached_last_error_message(memc));
  }

  memcached_free(memc);

  return TEST_SUCCESS;
}

test_return_t regression_bug_1251482(memcached_st*)
{
  test::Memc memc("--server=localhost:5");

  memcached_behavior_set(&memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 0);

  for (size_t x= 0; x < 5; ++x)
  {
    size_t value_length;
    memcached_return_t rc;
    char *value= memcached_get(&memc,
                               test_literal_param(__func__),
                               &value_length, NULL, &rc);

    test_false(value);
    test_compare(0LLU, value_length);
    if (x) {
      test_ne_compare(MEMCACHED_SUCCESS, rc);
    } else {
      test_compare(MEMCACHED_CONNECTION_FAILURE, rc);
    }
  }

  return TEST_SUCCESS;
}

test_return_t regression_1009493_TEST(memcached_st*)
{
  memcached_st* memc= memcached_create(NULL);
  test_true(memc);
  test_compare(MEMCACHED_SUCCESS, memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_KETAMA, true));

  memcached_st* clone= memcached_clone(NULL, memc);
  test_true(clone);

  test_compare(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED),
               memcached_behavior_get(clone, MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED));

  memcached_free(memc);
  memcached_free(clone);

  return TEST_SUCCESS;
}

test_return_t regression_994772_TEST(memcached_st* memc)
{
  test_skip(MEMCACHED_SUCCESS, memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1));

  test_compare(MEMCACHED_SUCCESS,
               memcached_set(memc,
                             test_literal_param(__func__), // Key
                             test_literal_param(__func__), // Value
                             time_t(0), uint32_t(0)));

  const char *keys[] = { __func__ };
  size_t key_length[]= { strlen(__func__) };
  test_compare(MEMCACHED_SUCCESS,
               memcached_mget(memc, keys, key_length, 1));

  memcached_return_t rc;
  memcached_result_st *results= memcached_fetch_result(memc, NULL, &rc);
  test_true(results);
  test_compare(MEMCACHED_SUCCESS, rc);

  test_strcmp(__func__, memcached_result_value(results));
  uint64_t cas_value= memcached_result_cas(results);
  test_true(cas_value);

  char* take_value= memcached_result_take_value(results);
  test_strcmp(__func__, take_value);
  free(take_value);

  memcached_result_free(results);

  // Bad cas value, sanity check
  test_true(cas_value != 9999);
  test_compare(MEMCACHED_END,
               memcached_cas(memc,
                             test_literal_param(__func__), // Key
                             test_literal_param(__FILE__), // Value
                             time_t(0), uint32_t(0), 9999));

  test_compare(MEMCACHED_SUCCESS, memcached_set(memc,
                                                "different", strlen("different"), // Key
                                                test_literal_param(__FILE__), // Value
                                                time_t(0), uint32_t(0)));

  return TEST_SUCCESS;
}
