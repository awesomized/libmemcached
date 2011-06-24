/* uTest
 * Copyright (C) 2011 Data Differential, http://datadifferential.com/
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */


#include <libtest/common.h>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctime>
#include <fnmatch.h>
#include <iostream>

#include <libtest/stats.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

static in_port_t global_port= 0;

in_port_t default_port()
{
  assert(global_port);
  return global_port;
}
 
void set_default_port(in_port_t port)
{
  global_port= port;
}

static void stats_print(Stats *stats)
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

  case TEST_MEMORY_ALLOCATION_FAILURE:
    return "memory allocation";

  case TEST_SKIPPED:
    return "skipped";

  case TEST_FATAL:
    break;
  }

  return "failed";
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
  {
    return func(p);
  }

  return TEST_SUCCESS;
}

static Runner defualt_runners= {
  _runner_default,
  _runner_default,
  _runner_default
};

static test_return_t _default_callback(void *p)
{
  (void)p;

  return TEST_SUCCESS;
}

Framework::Framework() :
  collections(NULL),
  _create(NULL),
  _destroy(NULL),
  collection_startup(_default_callback),
  collection_shutdown(_default_callback),
  _on_error(NULL),
  runner(&defualt_runners)
{
}


int main(int argc, char *argv[])
{
  Framework world;

  Stats stats;

  get_world(&world);

  if (not world.runner)
  {
    world.runner= &defualt_runners;
  }

  test_return_t error;
  void *world_ptr= world.create(&error);
  if (test_failed(error))
  {
    return EXIT_FAILURE;
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

  for (collection_st *next= world.collections; next->name; next++)
  {
    test_return_t collection_rc= TEST_SUCCESS;
    bool failed= false;
    bool skipped= false;

    if (collection_to_run && fnmatch(collection_to_run, next->name, 0))
      continue;

    stats.collection_total++;

    collection_rc= world.startup(world_ptr);

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
      test_assert(0, "Allocation failure, or unknown return");
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

      world.item.startup(world_ptr);

      world.item.flush(world_ptr, run);

      world.item.pre(world_ptr);

      test_return_t return_code;
      { // Runner Code
	gettimeofday(&start_time, NULL);
	return_code= world.runner->run(run->test_fn, world_ptr);
	gettimeofday(&end_time, NULL);
	load_time= timedif(end_time, start_time);
      }

      world.item.post(world_ptr);

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
	test_assert(0, "Memory Allocation Error");
      }

      std::cerr << "[ " << test_strerror(return_code) << " ]" << std::endl;

      if (test_failed(world.on_error(return_code, world_ptr)))
      {
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

    world.shutdown(world_ptr);
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

  if (test_failed(world.destroy(world_ptr)))
  {
    stats.failed++; // We do this to make our exit code return EXIT_FAILURE
  }

  stats_print(&stats);

  return stats.failed == 0 ? 0 : 1;
}
