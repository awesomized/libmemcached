/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Test memcat
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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
  Test that we are cycling the servers we are creating during testing.
*/

#include <config.h>

#include <libtest/test.hpp>
#include <libmemcached-1.0/memcached.h>

#include "tests/libmemcached-1.0/memcached_get.h"

using namespace libtest;

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

static std::string executable("example/memcached_light");

static test_return_t help_TEST(void *)
{
  const char *args[]= { "--help", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t verbose_TEST(void *)
{
  const char *args[]= { "--help", "--verbose", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t daemon_TEST(void *)
{
  const char *args[]= { "--help", "--daemon", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t protocol_TEST(void *)
{
  const char *args[]= { "--help", "--protocol", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t version_TEST(void *)
{
  const char *args[]= { "--help", "--version", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t port_TEST(void *)
{
  const char *args[]= { "--help", "--port=9090", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t pid_file_TEST(void *)
{
  const char *args[]= { "--help", "--pid-file=/tmp/foo.pid", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t log_file_TEST(void *)
{
  const char *args[]= { "--help", "--log-file=/tmp/foo.log", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

static test_return_t max_connections_file_TEST(void *)
{
  const char *args[]= { "--help", "--max-connections=/tmp/foo.max_connections", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline(executable, args, true));

  return TEST_SUCCESS;
}

typedef test_return_t (*libmemcached_test_callback_fn)(memcached_st *);

static test_return_t _runner_default(libmemcached_test_callback_fn func, void *object)
{
  if (func)
  {
    test_true(object);
    test_return_t ret;
    try {
      ret= func((memcached_st*)object);
    }
    catch (std::exception& e)
    {
      libtest::Error << e.what();
      return TEST_FAILURE;
    }

    return ret;
  }

  return TEST_SUCCESS;
}

class MemcachedLightRunner : public libtest::Runner {
public:
  test_return_t run(test_callback_fn* func, void *object)
  {
    return _runner_default(libmemcached_test_callback_fn(func), object);
  }
};

test_st cmdline_option_TESTS[] ={
  {"--help", true, help_TEST },
  {"--verbose", true, verbose_TEST },
  {"--daemon", true, daemon_TEST },
  {"--protocol", true, protocol_TEST },
  {"--version", true, version_TEST },
  {"--port", true, port_TEST },
  {"--pid-file", true, pid_file_TEST },
  {"--log-file", true, log_file_TEST },
  {"--max-connections", true, max_connections_file_TEST },
  {0, 0, 0}
};

/* Clean the server before beginning testing */
test_st basic_TESTS[] ={
#if 0
  {"memcached_get()", true, (test_callback_fn*)get_test },
  {"memcached_get() test 2", false, (test_callback_fn*)get_test2 },
  {"memcached_get() test 3", false, (test_callback_fn*)get_test3 },
  {"memcached_get() test 4", false, (test_callback_fn*)get_test4 },
  {"memcached_get() test 5", false, (test_callback_fn*)get_test5 },
#endif
  {0, 0, 0}
};

collection_st collection[] ={
  {"command line options", 0, 0, cmdline_option_TESTS },
  {"basic", 0, 0, basic_TESTS },
  {0, 0, 0, 0}
};

static void *world_create(server_startup_st& servers, test_return_t& error)
{
  if (access(executable.c_str(), X_OK) != 0)
  {
    error= TEST_SKIPPED;
    return NULL;
  }

  if (HAVE_MEMCACHED_LIGHT_BINARY == 0)
  {
    error= TEST_SKIPPED;
    return NULL;
  }

  if (server_startup(servers, "memcached-light", libtest::default_port(), 0, NULL) == 0)
  {
    error= TEST_FAILURE;
    return NULL;
  }


  char buffer[1024];
  int length= snprintf(buffer, sizeof(buffer), "--server=localhost:%d", int(libtest::default_port()));
  fatal_assert(length > 0);

  memcached_st *memc= memcached(buffer, length);

  fatal_assert(memc);

  return (void*)memc;
}

static bool world_destroy(void *object)
{
  memcached_st *memc= (memcached_st*)object;
  memcached_free(memc);

  return TEST_SUCCESS;
}


void get_world(libtest::Framework* world)
{
  world->create(world_create);
  world->destroy(world_destroy);
  world->collections(collection);
  world->set_runner(new MemcachedLightRunner);
}

