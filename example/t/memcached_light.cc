/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Test memslap
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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
#include <libmemcached/memcached.h>

using namespace libtest;

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

static std::string executable;

static test_return_t help_TEST(void *)
{
  const char *memcached_light_args[]= { "--help", 0 };
  Application memcached_light_app(executable, true);

  test_compare(Application::SUCCESS, memcached_light_app.run(memcached_light_args));
  test_compare(Application::SUCCESS, memcached_light_app.wait());

  return TEST_SUCCESS;
}

static test_return_t basic_TEST(void *)
{
  char port_buffer[1024];
  snprintf(port_buffer, sizeof(port_buffer), "--port=%d", int(libtest::default_port()));
  const char *memcached_light_args[]= { port_buffer, 0 };
  Application memcached_light_app(executable, true);

  test_compare(Application::SUCCESS, memcached_light_app.run(memcached_light_args));

  char instance_buffer[1024];
  int instance_buffer_length= snprintf(instance_buffer, sizeof(instance_buffer), "--BINARY --server=localhost:%d", int(libtest::default_port()));
  memcached_st *memc= memcached(instance_buffer, instance_buffer_length);

#if 0
  for (size_t x= 0; x < 24; x++)
  {
    char id_buffer[1024];
    int length= snprintf(id_buffer, sizeof(id_buffer), "%u", uint32_t(x));
    test_compare_hint(MEMCACHED_NOTFOUND, memcached_delete(memc, id_buffer, length, 0),
                      id_buffer);
  }
#endif

  // We fail since we are just outright killing it.
  test_compare(Application::FAILURE, memcached_light_app.wait());

  memcached_free(memc);

  return TEST_SUCCESS;
}

test_st help_TESTS[] ={
  {"--help", true, help_TEST },
  {0, 0, 0}
};

test_st basic_TESTS[] ={
  {"--port", true, basic_TEST },
  {0, 0, 0}
};

collection_st collection[] ={
  {"--help", 0, 0, help_TESTS },
  {"basics", 0, 0, basic_TESTS },
  {0, 0, 0, 0}
};

static void *world_create(server_startup_st& servers, test_return_t& error)
{
  if (HAVE_MEMCACHED_BINARY == 0)
  {
    error= TEST_SKIPPED;
    return NULL;
  }

  return &servers;
}


void get_world(Framework *world)
{
  executable= "./example/memcached_light";
  world->collections= collection;
  world->_create= world_create;
}


