/* uTest
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/*
  Sample test application.
*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <fnmatch.h>

#include "test.h"

static void world_stats_print(world_stats_st *stats)
{
  fprintf(stderr, "Total\t\t\t\t%u\n", stats->total);
  fprintf(stderr, "\tFailed\t\t\t%u\n", stats->failed);
  fprintf(stderr, "\tSkipped\t\t\t%u\n", stats->skipped);
  fprintf(stderr, "\tSucceeded\t\t%u\n", stats->success);
}

static long int timedif(struct timeval a, struct timeval b)
{
  register int us, s;

  us = (int)(a.tv_usec - b.tv_usec);
  us /= 1000;
  s = (int)(a.tv_sec - b.tv_sec);
  s *= 1000;
  return s + us;
}

const char *test_strerror(test_return_t code)
{
  switch (code) {
  case TEST_SUCCESS:
    return "ok";
  case TEST_FAILURE:
    return "failed";
  case TEST_MEMORY_ALLOCATION_FAILURE:
    return "memory allocation";
  case TEST_SKIPPED:
    return "skipped";
  case TEST_MAXIMUM_RETURN:
  default:
    fprintf(stderr, "Unknown return value\n");
    abort();
  }
}

void create_core(void)
{
  if (getenv("LIBMEMCACHED_NO_COREDUMP") == NULL)
  {
    pid_t pid= fork();

    if (pid == 0)
    {
      abort();
    }
    else
    {
      while (waitpid(pid, NULL, 0) != pid)
      {
        ;
      }
    }
  }
}


static test_return_t _runner_default(test_callback_fn func, void *p)
{
  if (func)
  {
    return func(p);
  }
  else
  {
    return TEST_SUCCESS;
  }
}

static world_runner_st defualt_runners= {
  _runner_default,
  _runner_default,
  _runner_default
};


int main(int argc, char *argv[])
{
  test_return_t return_code;
  unsigned int x;
  char *collection_to_run= NULL;
  char *wildcard= NULL;
  world_st world;
  collection_st *collection;
  collection_st *next;
  void *world_ptr;

  world_stats_st stats;

  memset(&stats, 0, sizeof(stats));
  memset(&world, 0, sizeof(world));
  get_world(&world);

  if (! world.runner)
  {
    world.runner= &defualt_runners;
  }

  collection= world.collections;

  if (world.create)
  {
    test_return_t error;
    world_ptr= world.create(&error);
    if (error != TEST_SUCCESS)
      exit(1);
  }
  else
  {
    world_ptr= NULL;
  }

  if (argc > 1)
    collection_to_run= argv[1];

  if (argc == 3)
    wildcard= argv[2];

  for (next= collection; next->name; next++)
  {
    test_st *run;

    run= next->tests;
    if (collection_to_run && fnmatch(collection_to_run, next->name, 0))
      continue;

    fprintf(stderr, "\n%s\n\n", next->name);

    for (x= 0; run->name; run++)
    {
      struct timeval start_time, end_time;
      long int load_time= 0;

      if (wildcard && fnmatch(wildcard, run->name, 0))
        continue;

      fprintf(stderr, "Testing %s", run->name);

      if (world.collection_startup)
      {
        world.collection_startup(world_ptr);
      }

      if (run->requires_flush && world.flush)
      {
        world.flush(world_ptr);
      }

      if (world.pre_run)
      {
        world.pre_run(world_ptr);
      }


      if (next->pre && world.runner->pre)
      {
        return_code= world.runner->pre(next->pre, world_ptr);

        if (return_code != TEST_SUCCESS)
        {
          goto error;
        }
      }

      gettimeofday(&start_time, NULL);
      return_code= world.runner->run(run->test_fn, world_ptr);
      gettimeofday(&end_time, NULL);
      load_time= timedif(end_time, start_time);

      if (next->post && world.runner->post)
      {
        (void) world.runner->post(next->post, world_ptr);
      }

      if (world.post_run)
      {
        world.post_run(world_ptr);
      }

error:
      stats.total++;

      fprintf(stderr, "\t\t\t\t\t");

      switch (return_code)
      {
      case TEST_SUCCESS:
        fprintf(stderr, "%ld.%03ld ", load_time / 1000, load_time % 1000);
        stats.success++;
        break;
      case TEST_FAILURE:
        stats.failed++;
        break;
      case TEST_SKIPPED:
        stats.skipped++;
        break;
      case TEST_MEMORY_ALLOCATION_FAILURE:
      case TEST_MAXIMUM_RETURN:
      default:
        break;
      }

      fprintf(stderr, "[ %s ]\n", test_strerror(return_code));

      if (world.on_error)
      {
        test_return_t rc;
        rc= world.on_error(return_code, world_ptr);

        if (rc != TEST_SUCCESS)
          break;
      }
    }
  }

  fprintf(stderr, "All tests completed successfully\n\n");

  if (world.destroy)
  {
    test_return_t error;
    error= world.destroy(world_ptr);

    if (error != TEST_SUCCESS)
    {
      fprintf(stderr, "Failure during shutdown.\n");
      stats.failed++; // We do this to make our exit code return 1
    }
  }

  world_stats_print(&stats);

  return stats.failed == 0 ? 0 : 1;
}
