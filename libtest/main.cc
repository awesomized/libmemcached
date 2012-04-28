/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Data Differential YATL (i.e. libtest)  library
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

#include <config.h>
#include <libtest/common.h>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fnmatch.h>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

static test_return_t runner_code(Framework* frame,
                                 test_st* run, 
                                 void* creators_ptr, 
                                 long int& load_time)
{ // Runner Code

  struct timeval start_time, end_time;

  gettimeofday(&start_time, NULL);
  assert(frame->runner());
  assert(run->test_fn);

  test_return_t return_code;
  try 
  {
    return_code= frame->runner()->run(run->test_fn, creators_ptr);
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

  return return_code;
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

  try 
  {
    do
    {
      exit_code= EXIT_SUCCESS;
      std::auto_ptr<Framework> frame(new Framework);

      fatal_assert(sigignore(SIGPIPE) == 0);

      libtest::SignalThread signal;
      if (signal.setup() == false)
      {
        Error << "Failed to setup signals";
        return EXIT_FAILURE;
      }

      Stats stats;

      get_world(frame.get());

      test_return_t error;
      void *creators_ptr= frame->create(error);

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

      for (collection_st *next= frame->collections; next and next->name and (not signal.is_shutdown()); next++)
      {
        if (collection_to_run.empty() == false and fnmatch(collection_to_run.c_str(), next->name, 0))
        {
          continue;
        }

        stats.collection_total++;

        bool failed= false;
        bool skipped= false;
        test_return_t collection_rc;
        if (test_success(collection_rc= frame->runner()->pre(next->pre, creators_ptr)))
        {
          Out << "Collection: " << next->name;

          for (test_st *run= next->tests; run->name; run++)
          {
            long int load_time= 0;

            if (wildcard && fnmatch(wildcard, run->name, 0))
            {
              continue;
            }

            test_return_t return_code;
            try 
            {
              if (run->requires_flush)
              {
                if (test_failed(frame->runner()->flush(creators_ptr)))
                {
                  Error << "frame->runner()->flush(creators_ptr)";
                  continue;
                }
              }

              return_code= runner_code(frame.get(), run, creators_ptr, load_time);

              if (return_code == TEST_SKIPPED)
              { }
              else if (return_code == TEST_FAILURE)
              {
#if 0
                Error << " frame->runner()->run(failure)";
                signal.set_shutdown(SHUTDOWN_GRACEFUL);
#endif
              }

            }
            catch (libtest::fatal &e)
            {
              Error << "Fatal exception was thrown: " << e.what();
              return_code= TEST_FAILURE;
              throw;
            }
            catch (std::exception &e)
            {
              Error << "Exception was thrown: " << e.what();
              return_code= TEST_FAILURE;
              throw;
            }
            catch (...)
            {
              Error << "Unknown exception occurred";
              return_code= TEST_FAILURE;
              throw;
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
#if 0
            @TODO add code here to allow for a collection to define a method to reset to allow tests to continue.
#endif
          }

          (void) frame->runner()->post(next->post, creators_ptr);
        }
        else if (collection_rc == TEST_FAILURE)
        {
          Out << next->name << " [ failed ]";
          failed= true;
#if 0
          signal.set_shutdown(SHUTDOWN_GRACEFUL);
#endif
        }
        else if (collection_rc == TEST_SKIPPED)
        {
          Out << next->name << " [ skipping ]";
          skipped= true;
        }

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

        Outn();
      }

      if (signal.is_shutdown() == false)
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
      else if (stats.collection_success and (stats.collection_failed == 0))
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
  catch (libtest::disconnected& e)
  {
    std::cerr << "Unhandled disconnection occurred:" << e.what() << std::endl;
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
