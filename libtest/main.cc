/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <config.h>
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

#include <signal.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

using namespace libtest;

static void stats_print(Stats *stats)
{
  if (stats->collection_failed == 0 and stats->collection_success == 0)
  {
    return;
  }

  Out << "\tTotal Collections\t\t\t\t" << stats->collection_total;
  Out << "\tFailed Collections\t\t\t\t" << stats->collection_failed;
  Out << "\tSkipped Collections\t\t\t\t" << stats->collection_skipped;
  Out << "\tSucceeded Collections\t\t\t\t" << stats->collection_success;
  Outn();
  Out << "Total\t\t\t\t" << stats->total;
  Out << "\tFailed\t\t\t" << stats->failed;
  Out << "\tSkipped\t\t\t" << stats->skipped;
  Out << "\tSucceeded\t\t" << stats->success;
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

#include <getopt.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  bool opt_massive= false;
  unsigned long int opt_repeat= 1; // Run all tests once
  bool opt_quiet= false;
  std::string collection_to_run;

  // Options parsing
  {
    enum long_option_t {
      OPT_LIBYATL_VERSION,
      OPT_LIBYATL_MATCH_COLLECTION,
      OPT_LIBYATL_MASSIVE,
      OPT_LIBYATL_QUIET,
      OPT_LIBYATL_REPEAT
    };

    static struct option long_options[]=
    {
      { "version", no_argument, NULL, OPT_LIBYATL_VERSION },
      { "quiet", no_argument, NULL, OPT_LIBYATL_QUIET },
      { "repeat", no_argument, NULL, OPT_LIBYATL_REPEAT },
      { "collection", required_argument, NULL, OPT_LIBYATL_MATCH_COLLECTION },
      { "massive", no_argument, NULL, OPT_LIBYATL_MASSIVE },
      { 0, 0, 0, 0 }
    };

    int option_index= 0;
    while (1)
    {
      int option_rv= getopt_long(argc, argv, "", long_options, &option_index);
      if (option_rv == -1)
      {
        break;
      }

      switch (option_rv)
      {
      case OPT_LIBYATL_VERSION:
        break;

      case OPT_LIBYATL_QUIET:
        opt_quiet= true;
        break;

      case OPT_LIBYATL_REPEAT:
        opt_repeat= strtoul(optarg, (char **) NULL, 10);
        break;

      case OPT_LIBYATL_MATCH_COLLECTION:
        collection_to_run= optarg;
        break;

      case OPT_LIBYATL_MASSIVE:
        opt_massive= true;
        break;

      case '?':
        /* getopt_long already printed an error message. */
        Error << "unknown option to getopt_long()";
        exit(EXIT_FAILURE);

      default:
        break;
      }
    }
  }

  srandom((unsigned int)time(NULL));

  if (bool(getenv("YATL_REPEAT")) and (strtoul(getenv("YATL_REPEAT"), (char **) NULL, 10) > 1))
  {
    opt_repeat= strtoul(getenv("YATL_REPEAT"), (char **) NULL, 10);
  }

  if ((bool(getenv("YATL_QUIET")) and (strcmp(getenv("YATL_QUIET"), "0") == 0)) or opt_quiet)
  {
    opt_quiet= true;
  }
  else if (getenv("JENKINS_URL"))
  {
    if (bool(getenv("YATL_QUIET")) and (strcmp(getenv("YATL_QUIET"), "1") == 0))
    { }
    else
    {
      opt_quiet= true;
    }
  }

  if (opt_quiet)
  {
    close(STDOUT_FILENO);
  }

  char buffer[1024];
  if (getenv("LIBTEST_TMP"))
  {
    snprintf(buffer, sizeof(buffer), "%s", getenv("LIBTEST_TMP"));
  }
  else
  {
    snprintf(buffer, sizeof(buffer), "%s", LIBTEST_TEMP);
  }

  if (chdir(buffer) == -1)
  {
    char getcwd_buffer[1024];
    char *dir= getcwd(getcwd_buffer, sizeof(getcwd_buffer));

    Error << "Unable to chdir() from " << dir << " to " << buffer << " errno:" << strerror(errno);
    return EXIT_FAILURE;
  }

  if (libtest::libtool() == NULL)
  {
    Error << "Failed to locate libtool";
    return EXIT_FAILURE;
  }

  int exit_code;

  try {
    do {
      exit_code= EXIT_SUCCESS;
      Framework world;

      fatal_assert(sigignore(SIGPIPE) == 0);

      libtest::SignalThread signal;
      if (signal.setup() == false)
      {
        Error << "Failed to setup signals";
        return EXIT_FAILURE;
      }

      Stats stats;

      get_world(&world);

      test_return_t error;
      void *creators_ptr= world.create(error);

      switch (error)
      {
      case TEST_SUCCESS:
        break;

      case TEST_SKIPPED:
        Out << "SKIP " << argv[0];
        return EXIT_SUCCESS;

      case TEST_FAILURE:
        return EXIT_FAILURE;
      }

      if (getenv("YATL_COLLECTION_TO_RUN"))
      {
        if (strlen(getenv("YATL_COLLECTION_TO_RUN")))
        {
          collection_to_run= getenv("YATL_COLLECTION_TO_RUN");
        }
      }

      if (collection_to_run.compare("none") == 0)
      {
        return EXIT_SUCCESS;
      }

      if (collection_to_run.empty() == false)
      {
        Out << "Only testing " <<  collection_to_run;
      }

      char *wildcard= NULL;
      if (argc == 3)
      {
        wildcard= argv[2];
      }

      for (collection_st *next= world.collections; next and next->name and (not signal.is_shutdown()); next++)
      {
        bool failed= false;
        bool skipped= false;

        if (collection_to_run.empty() == false and fnmatch(collection_to_run.c_str(), next->name, 0))
        {
          continue;
        }

        stats.collection_total++;

        test_return_t collection_rc= world.startup(creators_ptr);

        if (collection_rc == TEST_SUCCESS and next->pre)
        {
          collection_rc= world.runner()->pre(next->pre, creators_ptr);
        }

        switch (collection_rc)
        {
        case TEST_SUCCESS:
          break;

        case TEST_FAILURE:
          Out << next->name << " [ failed ]";
          failed= true;
          signal.set_shutdown(SHUTDOWN_GRACEFUL);
          goto cleanup;

        case TEST_SKIPPED:
          Out << next->name << " [ skipping ]";
          skipped= true;
          goto cleanup;

        default:
          fatal_message("invalid return code");
        }

        Out << "Collection: " << next->name;

        for (test_st *run= next->tests; run->name; run++)
        {
          struct timeval start_time, end_time;
          long int load_time= 0;

          if (wildcard && fnmatch(wildcard, run->name, 0))
          {
            continue;
          }

          test_return_t return_code;
          try {
            if (test_success(return_code= world.item.startup(creators_ptr)))
            {
              if (test_success(return_code= world.item.flush(creators_ptr, run)))
              {
                // @note pre will fail is SKIPPED is returned
                if (test_success(return_code= world.item.pre(creators_ptr)))
                {
                  { // Runner Code
                    gettimeofday(&start_time, NULL);
                    assert(world.runner());
                    assert(run->test_fn);
                    try 
                    {
                      return_code= world.runner()->run(run->test_fn, creators_ptr);
                    }
                    // Special case where check for the testing of the exception
                    // system.
                    catch (libtest::fatal &e)
                    {
                      if (fatal::is_disabled())
                      {
                        fatal::increment_disabled_counter();
                        return_code= TEST_SUCCESS;
                      }
                      else
                      {
                        throw;
                      }
                    }

                    gettimeofday(&end_time, NULL);
                    load_time= timedif(end_time, start_time);
                  }
                }

                // @todo do something if post fails
                (void)world.item.post(creators_ptr);
              }
              else if (return_code == TEST_SKIPPED)
              { }
              else if (return_code == TEST_FAILURE)
              {
                Error << " item.flush(failure)";
                signal.set_shutdown(SHUTDOWN_GRACEFUL);
              }
            }
            else if (return_code == TEST_SKIPPED)
            { }
            else if (return_code == TEST_FAILURE)
            {
              Error << " item.startup(failure)";
              signal.set_shutdown(SHUTDOWN_GRACEFUL);
            }
          }

          catch (libtest::fatal &e)
          {
            Error << "Fatal exception was thrown: " << e.what();
            return_code= TEST_FAILURE;
          }
          catch (std::exception &e)
          {
            Error << "Exception was thrown: " << e.what();
            return_code= TEST_FAILURE;
          }
          catch (...)
          {
            Error << "Unknown exception occurred";
            return_code= TEST_FAILURE;
          }

          stats.total++;

          switch (return_code)
          {
          case TEST_SUCCESS:
            Out << "\tTesting " << run->name <<  "\t\t\t\t\t" << load_time / 1000 << "." << load_time % 1000 << "[ " << test_strerror(return_code) << " ]";
            stats.success++;
            break;

          case TEST_FAILURE:
            stats.failed++;
            failed= true;
            Out << "\tTesting " << run->name <<  "\t\t\t\t\t" << "[ " << test_strerror(return_code) << " ]";
            break;

          case TEST_SKIPPED:
            stats.skipped++;
            skipped= true;
            Out << "\tTesting " << run->name <<  "\t\t\t\t\t" << "[ " << test_strerror(return_code) << " ]";
            break;

          default:
            fatal_message("invalid return code");
          }

          if (test_failed(world.on_error(return_code, creators_ptr)))
          {
            Error << "Failed while running on_error()";
            signal.set_shutdown(SHUTDOWN_GRACEFUL);
            break;
          }
        }

        (void) world.runner()->post(next->post, creators_ptr);

cleanup:
        if (failed == false and skipped == false)
        {
          stats.collection_success++;
        }

        if (failed)
        {
          stats.collection_failed++;
        }

        if (skipped)
        {
          stats.collection_skipped++;
        }

        world.shutdown(creators_ptr);
        Outn();
      }

      if (not signal.is_shutdown())
      {
        signal.set_shutdown(SHUTDOWN_GRACEFUL);
      }

      shutdown_t status= signal.get_shutdown();
      if (status == SHUTDOWN_FORCED)
      {
        Out << "Tests were aborted.";
        exit_code= EXIT_FAILURE;
      }
      else if (stats.collection_failed)
      {
        Out << "Some test failed.";
        exit_code= EXIT_FAILURE;
      }
      else if (stats.collection_skipped and stats.collection_failed and stats.collection_success)
      {
        Out << "Some tests were skipped.";
      }
      else if (stats.collection_success and stats.collection_failed == 0)
      {
        Out << "All tests completed successfully.";
      }

      stats_print(&stats);

      Outn(); // Generate a blank to break up the messages if make check/test has been run
    } while (exit_code == EXIT_SUCCESS and --opt_repeat);
  }
  catch (libtest::fatal& e)
  {
    std::cerr << e.what() << std::endl;
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "Unknown exception halted execution." << std::endl;
  }

  return exit_code;
}
