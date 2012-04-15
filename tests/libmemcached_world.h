/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached Client and Server 
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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


#pragma once

/* The structure we use for the test system */
struct libmemcached_test_container_st
{
  libtest::server_startup_st& construct;
  memcached_st *parent;
  memcached_st *memc;

  libmemcached_test_container_st(libtest::server_startup_st &construct_arg) :
    construct(construct_arg),
    parent(NULL),
    memc(NULL)
  { }
};

static void *world_create(libtest::server_startup_st& servers, test_return_t& error)
{
  if (HAVE_MEMCACHED_BINARY == 0)
  {
    error= TEST_SKIPPED;
    return NULL;
  }

  if (servers.sasl() and (LIBMEMCACHED_WITH_SASL_SUPPORT == 0 or MEMCACHED_SASL_BINARY == 0))
  {
    error= TEST_SKIPPED;
    return NULL;
  }

  // Assume we are running under valgrind, and bail
  if (servers.sasl() and getenv("TESTS_ENVIRONMENT"))
  {
    error= TEST_SKIPPED;
    return NULL;
  }

  for (uint32_t x= 0; x < servers.servers_to_run(); x++)
  {
    in_port_t port= libtest::get_free_port();

    if (servers.sasl())
    {
      if (server_startup(servers, "memcached-sasl", port, 0, NULL) == false)
      {
        fatal_message("Could not start memcached-sasl");
      }
    }
    else
    {
      if (server_startup(servers, "memcached", port, 0, NULL) == false)
      {
        fatal_message("Could not start memcached");
      }
    }
  }

  libmemcached_test_container_st *global_container= new libmemcached_test_container_st(servers);

  return global_container;
}

static test_return_t world_container_startup(libmemcached_test_container_st *container)
{
  char buffer[BUFSIZ];

  test_compare_got(MEMCACHED_SUCCESS,
                   libmemcached_check_configuration(container->construct.option_string().c_str(), container->construct.option_string().size(),
                                                    buffer, sizeof(buffer)),
                   container->construct.option_string().c_str());

  test_null(container->parent);
  container->parent= memcached(container->construct.option_string().c_str(), container->construct.option_string().size());
  test_true(container->parent);
  test_compare(MEMCACHED_SUCCESS, memcached_version(container->parent));

  if (container->construct.sasl())
  {
    if (memcached_failed(memcached_behavior_set(container->parent, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1)))
    {
      memcached_free(container->parent);
      return TEST_FAILURE;
    }

    if (memcached_failed(memcached_set_sasl_auth_data(container->parent, container->construct.username().c_str(), container->construct.password().c_str())))
    {
      memcached_free(container->parent);
      return TEST_FAILURE;
    }
  }

  return TEST_SUCCESS;
}

static test_return_t world_container_shutdown(libmemcached_test_container_st *container)
{
  memcached_free(container->parent);
  container->parent= NULL;

  return TEST_SUCCESS;
}

static test_return_t world_test_startup(libmemcached_test_container_st *container)
{
  test_true(container);
  test_null(container->memc);
  test_true(container->parent);
  container->memc= memcached_clone(NULL, container->parent);
  test_true(container->memc);

  return TEST_SUCCESS;
}

test_return_t world_flush(libmemcached_test_container_st *container);
test_return_t world_flush(libmemcached_test_container_st *container)
{
  test_true(container->memc);
  memcached_flush(container->memc, 0);
  memcached_quit(container->memc);

  return TEST_SUCCESS;
}

static test_return_t world_pre_run(libmemcached_test_container_st *container)
{
  test_true(container->memc);
  for (uint32_t loop= 0; loop < memcached_server_list_count(container->memc->servers); loop++)
  {
    memcached_server_instance_st instance= memcached_server_instance_by_position(container->memc, loop);

    test_compare(-1, instance->fd);
    test_compare(0U, instance->cursor_active);
  }

  return TEST_SUCCESS;
}


static test_return_t world_post_run(libmemcached_test_container_st *container)
{
  test_true(container->memc);

  return TEST_SUCCESS;
}

static test_return_t world_on_error(test_return_t , libmemcached_test_container_st *container)
{
  test_true(container->memc);
  memcached_free(container->memc);
  container->memc= NULL;

  return TEST_SUCCESS;
}

static bool world_destroy(void *object)
{
  libmemcached_test_container_st *container= (libmemcached_test_container_st *)object;
#if defined(LIBMEMCACHED_WITH_SASL_SUPPORT) && LIBMEMCACHED_WITH_SASL_SUPPORT
  if (LIBMEMCACHED_WITH_SASL_SUPPORT)
  {
    sasl_done();
  }
#endif

  delete container;

  return TEST_SUCCESS;
}

typedef test_return_t (*libmemcached_test_callback_fn)(memcached_st *);

#include "tests/runner.h"

static LibmemcachedRunner defualt_libmemcached_runner;
