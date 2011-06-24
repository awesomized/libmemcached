/* libMemcached Functions Test
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Description: This is the startup bits for any libmemcached test.
 *
 */

#ifdef	__cplusplus
extern "C" {
#endif

/* The structure we use for the test system */
struct libmemcached_test_container_st
{
  server_startup_st construct;
  memcached_st *parent;
  memcached_st *memc;

  libmemcached_test_container_st() :
    parent(NULL),
    memc(NULL)
  {
    memset(&construct, 0, sizeof(server_startup_st));
  }
};

/* Prototypes for functions we will pass to test framework */
libmemcached_test_container_st *world_create(test_return_t *error);
test_return_t world_test_startup(libmemcached_test_container_st *);
test_return_t world_flush(libmemcached_test_container_st *container);
test_return_t world_pre_run(libmemcached_test_container_st *);

test_return_t world_post_run(libmemcached_test_container_st *);
test_return_t world_on_error(test_return_t, libmemcached_test_container_st *);
test_return_t world_destroy(libmemcached_test_container_st *);

static libmemcached_test_container_st global_container;

/**
  @note generic shutdown/startup for libmemcached tests.
*/
test_return_t world_container_startup(libmemcached_test_container_st *container);
test_return_t world_container_shutdown(libmemcached_test_container_st *container);

libmemcached_test_container_st *world_create(test_return_t *error)
{
  global_container.construct.count= SERVERS_TO_CREATE;
  global_container.construct.udp= 0;
  server_startup(&global_container.construct);

  if (not global_container.construct.servers)
  {
    *error= TEST_FAILURE;
    server_shutdown(&global_container.construct);
    return NULL;
  }

  *error= TEST_SUCCESS;

  return &global_container;
}

test_return_t world_container_startup(libmemcached_test_container_st *container)
{
  container->parent= memcached_create(NULL);
  test_true((container->parent != NULL));

  test_compare(MEMCACHED_SUCCESS,
	       memcached_server_push(container->parent, container->construct.servers));

  return TEST_SUCCESS;
}

test_return_t world_container_shutdown(libmemcached_test_container_st *container)
{
  memcached_free(container->parent);
  container->parent= NULL;

  return TEST_SUCCESS;
}

test_return_t world_test_startup(libmemcached_test_container_st *container)
{
  container->memc= memcached_clone(NULL, container->parent);
  test_true((container->memc != NULL));

  return TEST_SUCCESS;
}

test_return_t world_flush(libmemcached_test_container_st *container)
{
  memcached_flush(container->memc, 0);
  memcached_quit(container->memc);

  return TEST_SUCCESS;
}

test_return_t world_pre_run(libmemcached_test_container_st *container)
{
  for (uint32_t loop= 0; loop < memcached_server_list_count(container->memc->servers); loop++)
  {
    memcached_server_instance_st instance=
      memcached_server_instance_by_position(container->memc, loop);

    test_compare(-1, instance->fd);
    test_compare(0, instance->cursor_active);
  }

  return TEST_SUCCESS;
}


test_return_t world_post_run(libmemcached_test_container_st *container)
{
  test_true(container->memc);

  return TEST_SUCCESS;
}

test_return_t world_on_error(test_return_t test_state, libmemcached_test_container_st *container)
{
  (void)test_state;
  memcached_free(container->memc);
  container->memc= NULL;

  return TEST_SUCCESS;
}

test_return_t world_destroy(libmemcached_test_container_st *container)
{
  server_startup_st *construct= &container->construct;
  memcached_server_st *servers= container->construct.servers;
  memcached_server_list_free(servers);

  server_shutdown(construct);

#ifdef LIBMEMCACHED_WITH_SASL_SUPPORT
  sasl_done();
#endif

  return TEST_SUCCESS;
}

typedef test_return_t (*libmemcached_test_callback_fn)(memcached_st *);
static test_return_t _runner_default(libmemcached_test_callback_fn func, libmemcached_test_container_st *container)
{
  if (func)
  {
    return func(container->memc);
  }
  else
  {
    return TEST_SUCCESS;
  }
}

static test_return_t _pre_runner_default(libmemcached_test_callback_fn func, libmemcached_test_container_st *container)
{
  if (func)
  {
    return func(container->parent);
  }
  else
  {
    return TEST_SUCCESS;
  }
}

static test_return_t _post_runner_default(libmemcached_test_callback_fn func, libmemcached_test_container_st *container)
{
  if (func)
  {
    return func(container->parent);
  }
  else
  {
    return TEST_SUCCESS;
  }
}

#ifdef	__cplusplus
}
#endif

#ifdef	__cplusplus

static Runner defualt_libmemcached_runner= {
  reinterpret_cast<test_callback_runner_fn*>(_pre_runner_default),
  reinterpret_cast<test_callback_runner_fn*>(_runner_default),
  reinterpret_cast<test_callback_runner_fn*>(_post_runner_default)
};

#else

static Runner defualt_libmemcached_runner= {
  (test_callback_runner_fn)_pre_runner_default,
  (test_callback_runner_fn)_runner_default,
  (test_callback_runner_fn)_post_runner_default
};

#endif
