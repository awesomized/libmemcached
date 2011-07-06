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
#include <cerrno>

#include <signal.h>

#include <libtest/stats.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

static in_port_t global_port= 0;
static char global_socket[1024];

in_port_t default_port()
{
  assert(global_port);
  return global_port;
}
 
void set_default_port(in_port_t port)
{
  global_port= port;
}

const char *default_socket()
{
  assert(global_socket[0]);
  return global_socket;
}

bool test_is_local()
{
  return (getenv("LIBTEST_LOCAL"));
}

void set_default_socket(const char *socket)
{
  strncpy(global_socket, socket, strlen(socket));
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

enum shutdown_t {
  SHUTDOWN_RUNNING,
  SHUTDOWN_GRACEFUL,
  SHUTDOWN_FORCED
};

static Framework *world= NULL;
static volatile shutdown_t __shutdown= SHUTDOWN_RUNNING;

static void *sig_thread(void *arg)
{   
  sigset_t *set= (sigset_t *) arg;

  for (;__shutdown == SHUTDOWN_RUNNING;)
  {
    int sig;
    int error;
    while ((error= sigwait(set, &sig)) == EINTR) ;

    switch (sig)
    {
    case SIGSEGV:
    case SIGINT:
    case SIGABRT:
      std::cerr << std::endl << "Signal handling thread got signal " <<  strsignal(sig) << std::endl;
      __shutdown= SHUTDOWN_FORCED;
      break;

    default:
      std::cerr << std::endl << "Signal handling thread got unexpected signal " <<  strsignal(sig) << std::endl;
    case SIGUSR1:
      break;
    }
  }

  return NULL;
}


static void setup_signals(pthread_t& thread)
{
  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGSEGV);
  sigaddset(&set, SIGABRT);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGUSR1);

  int error;
  if ((error= pthread_sigmask(SIG_BLOCK, &set, NULL)) != 0)
  {
    std::cerr << __FILE__ << ":" << __LINE__ << " died during pthread_sigmask(" << strerror(error) << ")" << std::endl;
    exit(EXIT_FAILURE);
  }

  if ((error= pthread_create(&thread, NULL, &sig_thread, (void *) &set)) != 0)
  {
    std::cerr << __FILE__ << ":" << __LINE__ << " died during pthread_create(" << strerror(error) << ")" << std::endl;
    exit(EXIT_FAILURE);
  }
}


int main(int argc, char *argv[])
{
  world= new Framework();

  if (not world)
  {
    return EXIT_FAILURE;
  }

  pthread_t thread;
  setup_signals(thread);

  Stats stats;

  get_world(world);

  test_return_t error;
  void *creators_ptr= world->create(error);
  if (test_failed(error))
  {
    std::cerr << "create() failed" << std::endl;
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

  for (collection_st *next= world->collections; next->name and __shutdown == SHUTDOWN_RUNNING; next++)
  {
    test_return_t collection_rc= TEST_SUCCESS;
    bool failed= false;
    bool skipped= false;

    if (collection_to_run && fnmatch(collection_to_run, next->name, 0))
      continue;

    stats.collection_total++;

    collection_rc= world->startup(creators_ptr);

    if (collection_rc == TEST_SUCCESS and next->pre)
    {
      collection_rc= world->runner->pre(next->pre, creators_ptr);
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

      test_return_t return_code;
      if (test_success(return_code= world->item.startup(creators_ptr)))
      {
        if (test_success(return_code= world->item.flush(creators_ptr, run)))
        {
          // @note pre will fail is SKIPPED is returned
          if (test_success(return_code= world->item.pre(creators_ptr)))
          {
            { // Runner Code
              gettimeofday(&start_time, NULL);
              return_code= world->runner->run(run->test_fn, creators_ptr);
              gettimeofday(&end_time, NULL);
              load_time= timedif(end_time, start_time);
            }
          }

          // @todo do something if post fails
          (void)world->item.post(creators_ptr);
        }
        else
        {
          std::cerr << __FILE__ << ":" << __LINE__ << " item.flush(failure)" << std::endl;
        }
      }
      else
      {
        std::cerr << __FILE__ << ":" << __LINE__ << " item.startup(failure)" << std::endl;
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
	test_assert(0, "Memory Allocation Error");
      }

      std::cerr << "[ " << test_strerror(return_code) << " ]" << std::endl;

      if (test_failed(world->on_error(return_code, creators_ptr)))
      {
        break;
      }
    }

    if (next->post and world->runner->post)
    {
      (void) world->runner->post(next->post, creators_ptr);
    }

    if (failed == 0 and skipped == 0)
    {
      stats.collection_success++;
    }
cleanup:

    world->shutdown(creators_ptr);
  }

  if (__shutdown == SHUTDOWN_RUNNING)
  {
    __shutdown= SHUTDOWN_GRACEFUL;
  }

  if (__shutdown == SHUTDOWN_FORCED)
  {
    std::cerr << std::endl << std::endl <<  "Tests were aborted." << std::endl << std::endl;
  }
  else if (stats.collection_failed or stats.collection_skipped)
  {
    std::cerr << std::endl << std::endl <<  "Some test failures and/or skipped test occurred." << std::endl << std::endl;
  }
  else
  {
    std::cout << std::endl << std::endl <<  "All tests completed successfully." << std::endl << std::endl;
  }

  stats_print(&stats);

  void *retval;
  pthread_kill(thread, SIGUSR1);
  pthread_join(thread, &retval);

  delete world;

  return stats.failed == 0 and __shutdown == SHUTDOWN_GRACEFUL ? EXIT_SUCCESS : EXIT_FAILURE;
}
