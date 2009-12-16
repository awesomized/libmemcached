/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary:
 *
 */

/*
  Sample test application.
*/
#include <assert.h>
#include <libmemcached/memcached.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "test.h"
#include "server.h"

/* Prototypes */
test_return_t set_test(memcached_st *memc);

test_return_t set_test(memcached_st *memc)
{
  memcached_return_t rc;
  const char *key= "foo";
  const char *value= "when we sanitize";

  rc= memcached_set(memc, key, strlen(key),
                    value, strlen(value),
                    (time_t)0, (uint32_t)0);
  test_truth(rc == MEMCACHED_SUCCESS || rc == MEMCACHED_BUFFERED);

  return TEST_SUCCESS;
}

test_st tests[] ={
  {"set", 1, (test_callback_fn)set_test },
  {0, 0, 0}
};

collection_st collection[] ={
  {"udp", 0, 0, tests},
  {0, 0, 0, 0}
};

#define SERVERS_TO_CREATE 1

#include "libmemcached_world.h"

void get_world(world_st *world)
{
  world->collections= collection;
  world->collection_startup= (test_callback_fn)world_collection_startup;
  world->flush= (test_callback_fn)world_flush;
  world->pre_run= (test_callback_fn)world_pre_run;
  world->create= (test_callback_create_fn)world_create;
  world->post_run= (test_callback_fn)world_post_run;
  world->on_error= (test_callback_error_fn)world_on_error;
  world->destroy= (test_callback_fn)world_destroy;
  world->runner= &defualt_libmemcached_runner;
}
