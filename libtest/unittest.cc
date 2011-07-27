/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  uTest self unit test.
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

#include <libtest/test.hpp>

#include <cstdlib>
#include <unistd.h>

using namespace libtest;

static test_return_t LIBTOOL_COMMAND_test(void *)
{
  test_true(getenv("LIBTOOL_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t VALGRIND_COMMAND_test(void *)
{
  test_true(getenv("VALGRIND_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t HELGRIND_COMMAND_test(void *)
{
  test_true(getenv("HELGRIND_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t GDB_COMMAND_test(void *)
{
  test_true(getenv("GDB_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t test_success_test(void *)
{
  return TEST_SUCCESS;
}

static test_return_t test_failure_test(void *)
{
  return TEST_SKIPPED; // Only run this when debugging

  test_compare(1, 2);
  return TEST_SUCCESS;
}

static test_return_t local_test(void *)
{
  if (getenv("LIBTEST_LOCAL"))
  {
    test_true(test_is_local());
  }
  else
  {
    test_false(test_is_local());
  }

  return TEST_SUCCESS;
}

static test_return_t local_not_test(void *)
{
  return TEST_SKIPPED;

  std::string temp;

  const char *ptr;
  if ((ptr= getenv("LIBTEST_LOCAL")) == NULL)
  {
    temp.append(ptr);
  }

  // unsetenv() will cause issues with valgrind
  _compare(__FILE__, __LINE__, __func__, 0, unsetenv("LIBTEST_LOCAL"));
  test_compare(0, unsetenv("LIBTEST_LOCAL"));
  test_false(test_is_local());

  test_compare(0, setenv("LIBTEST_LOCAL", "1", 1));
  test_true(test_is_local());

  if (temp.empty())
  {
    test_compare(0, unsetenv("LIBTEST_LOCAL"));
  }
  else
  {
    char *old_string= strdup(temp.c_str());
    test_compare(0, setenv("LIBTEST_LOCAL", old_string, 1));
  }

  return TEST_SUCCESS;
}

#if 0
static test_return_t pause_test(void *)
{
  (void)getchar();
  return TEST_SUCCESS;
}
#endif


static test_return_t gearmand_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

#ifndef HAVE_LIBGEARMAN
  return TEST_SKIPPED;
#endif

  const char *argv[1]= { "cycle_gearmand" };
  test_true(server_startup(*servers, "gearmand", 9999, 1, argv));

  return TEST_SUCCESS;
}

static test_return_t memcached_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

#if !defined(MEMCACHED_BINARY) || !defined(HAVE_LIBMEMCACHED)
  return TEST_SKIPPED;
#endif

  const char *argv[1]= { "cycle_memcached" };
  test_true(server_startup(*servers, "memcached", 9998, 1, argv));

  return TEST_SUCCESS;
}

static test_return_t memcached_socket_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

#if !defined(MEMCACHED_BINARY) || !defined(HAVE_LIBMEMCACHED)
  return TEST_SKIPPED;
#endif

  const char *argv[1]= { "cycle_memcached" };
  test_true(servers->start_socket_server("memcached", 9997, 1, argv));

  return TEST_SUCCESS;
}

test_st gearmand_tests[] ={
#if 0
  {"pause", 0, pause_test },
#endif
  {"gearmand startup-shutdown", 0, gearmand_cycle_test },
  {0, 0, 0}
};

test_st memcached_tests[] ={
  {"memcached startup-shutdown", 0, memcached_cycle_test },
  {"memcached(socket file) startup-shutdown", 0, memcached_socket_cycle_test },
  {0, 0, 0}
};

test_st environment_tests[] ={
  {"LIBTOOL_COMMAND", 0, LIBTOOL_COMMAND_test },
  {"VALGRIND_COMMAND", 0, VALGRIND_COMMAND_test },
  {"HELGRIND_COMMAND", 0, HELGRIND_COMMAND_test },
  {"GDB_COMMAND", 0, GDB_COMMAND_test },
  {0, 0, 0}
};

test_st tests_log[] ={
  {"TEST_SUCCESS", 0, test_success_test },
  {"TEST_FAILURE", 0, test_failure_test },
  {0, 0, 0}
};

test_st local_log[] ={
  {"test_is_local()", 0, local_test },
  {"test_is_local(NOT)", 0, local_not_test },
  {0, 0, 0}
};

collection_st collection[] ={
  {"environment", 0, 0, environment_tests},
  {"return values", 0, 0, tests_log},
  {"local", 0, 0, local_log},
  {"gearmand", 0, 0, gearmand_tests},
  {"memcached", 0, 0, memcached_tests},
  {0, 0, 0, 0}
};

static void *world_create(server_startup_st& servers, test_return_t&)
{
  return &servers;
}

void get_world(Framework *world)
{
  world->collections= collection;
  world->_create= world_create;
}
