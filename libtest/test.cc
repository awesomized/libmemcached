/* uTest
 * Copyright (C) 2011 Data Differential, http://datadifferential.com/
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */


#include <config.h>

#include <stdint.h>
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
#include <iostream>

#include <libtest/test.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

static void world_stats_print(world_stats_st *stats)
{
  std::cout << "\tTotal Collections\t\t\t\t" << stats->collection_total << std::endl;
  std::cout << "\tFailed Collections\t\t\t\t" << stats->collection_failed << std::endl;
  std::cout << "\tSkipped Collections\t\t\t\t" << stats->collection_skipped << std::endl;
  std::cout << "\tSucceeded Collections\t\t\t\t" << stats->collection_success << std::endl;
  std::cout << std::endl;
  std::cout << "Total\t\t\t\t" << stats->total << std::endl;
  std::cout << "\tFailed\t\t\t" << stats->failed << std::endl;
  std::cout << "\tSkipped\t\t\t" << stats->skipped << std::endl;
  std::cout << "\tSucceeded\t\t" << stats->success << std::endl;
}

static long int timedif(struct timeval a, struct timeval b)
{
  long us, s;

  us = (long)(a.tv_usec - b.tv_usec);
  us /= 1000;
  s = (long)(a.tv_sec - b.tv_sec);
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
  case TEST_FATAL:
    return "failed";
  case TEST_MEMORY_ALLOCATION_FAILURE:
    return "memory allocation";
  case TEST_SKIPPED:
    return "skipped";
  case TEST_MAXIMUM_RETURN:
  default:
     std::cerr << "Unknown return value." << std::endl;
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
      while (waitpid(pid, NULL, 0) != pid) {};
    }
  }
}


static test_return_t _runner_default(test_callback_fn func, void *p)
{
  if (func)
    return func(p);

  return TEST_SUCCESS;
}

static world_runner_st defualt_runners= {
  _runner_default,
  _runner_default,
  _runner_default
};

static test_return_t _default_callback(void *p)
{
  (void)p;

  return TEST_SUCCESS;
}

static collection_st *init_world(world_st *world)
{
  if (world->runner == NULL)
  {
    world->runner= &defualt_runners;
  }

  if (world->collection_startup == NULL)
    world->collection_startup= _default_callback;

  if (world->collection_shutdown == NULL)
    world->collection_shutdown= _default_callback;

  return world->collections;
}


int main(int argc, char *argv[])
{
  world_st world;
  void *world_ptr;

  world_stats_st stats;

  get_world(&world);

  if (not world.runner)
  {
    world.runner= &defualt_runners;
  }

  collection_st *collection= init_world(&world);

  if (world.create)
  {
    test_return_t error;
    world_ptr= world.create(&error);
    if (error != TEST_SUCCESS)
    {
      return EXIT_FAILURE;
    }
  }
  else
  {
    world_ptr= NULL;
  }

  char *collection_to_run= NULL;
  if (argc > 1)
  {
    collection_to_run= argv[1];
  }
  else if (getenv("TEST_COLLECTION"))
  {
    collection_to_run= getenv("TEST_COLLECTION");
  }

  if (collection_to_run)
  {
    std::cout << "Only testing " <<  collection_to_run << std::endl;
  }

  char *wildcard= NULL;
  if (argc == 3)
  {
    wildcard= argv[2];
  }

  for (collection_st *next= collection; next->name; next++)
  {
    test_return_t collection_rc= TEST_SUCCESS;
    bool failed= false;
    bool skipped= false;

    if (collection_to_run && fnmatch(collection_to_run, next->name, 0))
      continue;

    stats.collection_total++;

    collection_rc= world.collection_startup(world_ptr);

    if (collection_rc == TEST_SUCCESS and next->pre)
    {
      collection_rc= world.runner->pre(next->pre, world_ptr);
    }

    switch (collection_rc)
    {
    case TEST_SUCCESS:
      std::cerr << std::endl << next->name << std::endl << std::endl;
      break;

    case TEST_FATAL:
    case TEST_FAILURE:
      std::cerr << std::endl << next->name << " [ failed ]" << std::endl << std::endl;
      stats.collection_failed++;
      goto cleanup;

    case TEST_SKIPPED:
      std::cerr << std::endl << next->name << " [ skipping ]" << std::endl << std::endl;
      stats.collection_skipped++;
      goto cleanup;

    case TEST_MEMORY_ALLOCATION_FAILURE:
    case TEST_MAXIMUM_RETURN:
    default:
      assert(0);
      break;
    }

    for (test_st *run= next->tests; run->name; run++)
    {
      struct timeval start_time, end_time;
      long int load_time= 0;

      if (wildcard && fnmatch(wildcard, run->name, 0))
      {
	continue;
      }

      std::cerr << "\tTesting " << run->name;

      if (world.run_startup)
      {
	world.run_startup(world_ptr);
      }

      if (run->requires_flush && world.flush)
      {
	world.flush(world_ptr);
      }

      if (world.pre_run)
      {
	world.pre_run(world_ptr);
      }


      test_return_t return_code;
      { // Runner Code
#if 0
	if (next->pre and world.runner->pre)
	{
	  return_code= world.runner->pre(next->pre, world_ptr);

	  if (return_code != TEST_SUCCESS)
	  {
	    goto error;
	  }
	}
#endif

	gettimeofday(&start_time, NULL);
	return_code= world.runner->run(run->test_fn, world_ptr);
	gettimeofday(&end_time, NULL);
	load_time= timedif(end_time, start_time);

#if 0
	if (next->post && world.runner->post)
	{
	  (void) world.runner->post(next->post, world_ptr);
	}
#endif
      }

      if (world.post_run)
      {
	world.post_run(world_ptr);
      }

      stats.total++;

      std::cerr << "\t\t\t\t\t";

      switch (return_code)
      {
      case TEST_SUCCESS:
	std::cerr << load_time / 1000 << "." << load_time % 1000;
	stats.success++;
	break;

      case TEST_FATAL:
      case TEST_FAILURE:
	stats.failed++;
	failed= true;
	break;

      case TEST_SKIPPED:
	stats.skipped++;
	skipped= true;
	break;

      case TEST_MEMORY_ALLOCATION_FAILURE:
      case TEST_MAXIMUM_RETURN:
      default:
	break;
	abort();
      }

      std::cerr << "[ " << test_strerror(return_code) << " ]" << std::endl;

      if (world.on_error)
      {
	test_return_t rc= world.on_error(return_code, world_ptr);

	if (rc != TEST_SUCCESS)
	  break;
      }

    }

    if (next->post && world.runner->post)
    {
      (void) world.runner->post(next->post, world_ptr);
    }

    if (failed == 0 and skipped == 0)
    {
      stats.collection_success++;
    }
cleanup:

    if (world.collection_shutdown)
    {
      world.collection_shutdown(world_ptr);
    }
  }

  if (stats.collection_failed || stats.collection_skipped)
  {
    std::cerr << std::endl << std::endl <<  "Some test failures and/or skipped test occurred." << std::endl << std::endl;
#if 0
    print_failed_test();
#endif
  }
  else
  {
    std::cout << std::endl << std::endl <<  "All tests completed successfully." << std::endl << std::endl;
  }

  if (world.destroy)
  {
    test_return_t error= world.destroy(world_ptr);

    if (error != TEST_SUCCESS)
    {
      std::cerr << "Failure during shutdown." << std::endl;
      stats.failed++; // We do this to make our exit code return EXIT_FAILURE
    }
  }

  world_stats_print(&stats);

  return stats.failed == 0 ? 0 : 1;
}
