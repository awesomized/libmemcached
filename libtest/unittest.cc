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

#include "libtest/yatlcon.h"

#include <libtest/test.hpp>

#if defined(HAVE_LIBMEMCACHED_1_0_TYPES_RETURN_H) && HAVE_LIBMEMCACHED_1_0_TYPES_RETURN_H
# include <libmemcached-1.0/types/return.h>
#endif

#if defined(HAVE_LIBGEARMAN_1_0_RETURN_H) && HAVE_LIBGEARMAN_1_0_RETURN_H
# include <libgearman-1.0/return.h>
#endif

#include <cstdlib>
#include <unistd.h>

using namespace libtest;

static std::string testing_service;

// Used to track setups where we see if failure is happening
static uint32_t fatal_calls= 0;

static test_return_t getenv_TEST(void *)
{
#if 0
  for (char **ptr= environ; *ptr; ptr++)
  {
    Error << *ptr;
  }
#endif

  return TEST_SUCCESS;
}

static test_return_t LIBTOOL_COMMAND_test(void *)
{
  test_true(getenv("LIBTOOL_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t VALGRIND_COMMAND_test(void *)
{
  test_true(getenv("VALGRIND_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t HELGRIND_COMMAND_test(void *)
{
  test_true(getenv("HELGRIND_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t GDB_COMMAND_test(void *)
{
  test_true(getenv("GDB_COMMAND"));
  return TEST_SUCCESS;
}

static test_return_t test_success_equals_one_test(void *)
{
  test_skip(HAVE_LIBMEMCACHED, 1);
#if defined(HAVE_LIBMEMCACHED_1_0_TYPES_RETURN_H) && HAVE_LIBMEMCACHED_1_0_TYPES_RETURN_H
  test_zero(MEMCACHED_SUCCESS);
#endif
  return TEST_SUCCESS;
}

static test_return_t test_success_test(void *)
{
  return TEST_SUCCESS;
}

static test_return_t test_throw_success_TEST(void *)
{
  try {
    _SUCCESS;
  }
  catch (libtest::__success)
  {
    return TEST_SUCCESS;
  }
  catch (...)
  {
    return TEST_FAILURE;
  }

  return TEST_FAILURE;
}

static test_return_t test_throw_skip_TEST(void *)
{
  try {
    SKIP;
  }
  catch (libtest::__skipped)
  {
    return TEST_SUCCESS;
  }
  catch (...)
  {
    return TEST_FAILURE;
  }

  return TEST_FAILURE;
}

static test_return_t test_throw_fail_TEST(void *)
{
  std::string error_messsage("test message!");
  try {
    FAIL(error_messsage);
  }
  catch (libtest::__failure e)
  {
    std::string compare_message("test message!");
    test_zero(compare_message.compare(e.what()));
    return TEST_SUCCESS;
  }
  catch (...)
  {
    return TEST_FAILURE;
  }

  return TEST_FAILURE;
}

static test_return_t test_failure_test(void *)
{
  return TEST_SKIPPED; // Only run this when debugging

  test_compare(1, 2);
  return TEST_SUCCESS;
}

static test_return_t local_test(void *)
{
  if (getenv("LIBTEST_LOCAL"))
  {
    test_true(test_is_local());
  }
  else
  {
    test_false(test_is_local());
  }

  return TEST_SUCCESS;
}

static test_return_t local_not_test(void *)
{
  return TEST_SKIPPED;

  std::string temp;

  const char *ptr;
  if ((ptr= getenv("LIBTEST_LOCAL")) == NULL)
  {
    temp.append(ptr);
  }

  // unsetenv() will cause issues with valgrind
  _compare(__FILE__, __LINE__, __func__, 0, unsetenv("LIBTEST_LOCAL"), true);
  test_compare(0, unsetenv("LIBTEST_LOCAL"));
  test_false(test_is_local());

  test_compare(0, setenv("LIBTEST_LOCAL", "1", 1));
  test_true(test_is_local());

  if (temp.empty())
  {
    test_compare(0, unsetenv("LIBTEST_LOCAL"));
  }
  else
  {
    char *old_string= strdup(temp.c_str());
    test_compare(0, setenv("LIBTEST_LOCAL", old_string, 1));
  }

  return TEST_SUCCESS;
}

static test_return_t var_exists_test(void *)
{
  test_compare(0, access("var", R_OK | W_OK | X_OK));
  return TEST_SUCCESS;
}

static test_return_t var_tmp_exists_test(void *)
{
  test_compare(0, access("var/tmp", R_OK | W_OK | X_OK));
  return TEST_SUCCESS;
}

static test_return_t var_run_exists_test(void *)
{
  test_compare(0, access("var/run", R_OK | W_OK | X_OK));
  return TEST_SUCCESS;
}

static test_return_t var_log_exists_test(void *)
{
  test_compare(0, access("var/log", R_OK | W_OK | X_OK));
  return TEST_SUCCESS;
}

static test_return_t var_drizzle_exists_test(void *)
{
  test_compare(0, access("var/drizzle", R_OK | W_OK | X_OK));
  return TEST_SUCCESS;
}

static test_return_t var_tmp_test(void *)
{
  FILE *file= fopen("var/tmp/junk", "w+");
  test_true(file);
  fclose(file);
  return TEST_SUCCESS;
}

static test_return_t var_run_test(void *)
{
  FILE *file= fopen("var/run/junk", "w+");
  test_true(file);
  fclose(file);
  return TEST_SUCCESS;
}

static test_return_t var_log_test(void *)
{
  FILE *file= fopen("var/log/junk", "w+");
  test_true(file);
  fclose(file);
  return TEST_SUCCESS;
}

static test_return_t var_drizzle_test(void *)
{
  FILE *file= fopen("var/drizzle/junk", "w+");
  test_true(file);
  fclose(file);
  return TEST_SUCCESS;
}

static test_return_t var_tmp_rm_test(void *)
{
  test_true(unlink("var/tmp/junk") == 0);
  return TEST_SUCCESS;
}

static test_return_t var_run_rm_test(void *)
{
  test_true(unlink("var/run/junk") == 0);
  return TEST_SUCCESS;
}

static test_return_t var_log_rm_test(void *)
{
  test_true(unlink("var/log/junk") == 0);
  return TEST_SUCCESS;
}

static test_return_t var_drizzle_rm_test(void *)
{
  test_true(unlink("var/drizzle/junk") == 0);
  return TEST_SUCCESS;
}

static test_return_t _compare_test_return_t_test(void *)
{
  test_compare(TEST_SUCCESS, TEST_SUCCESS);

  return TEST_SUCCESS;
}

static test_return_t _compare_memcached_return_t_test(void *)
{
  test_skip(HAVE_LIBMEMCACHED, true);
#if defined(HAVE_LIBMEMCACHED_1_0_TYPES_RETURN_H) && HAVE_LIBMEMCACHED_1_0_TYPES_RETURN_H
  test_compare(MEMCACHED_SUCCESS, MEMCACHED_SUCCESS);
#endif

  return TEST_SUCCESS;
}

static test_return_t _compare_gearman_return_t_test(void *)
{
  test_skip(HAVE_LIBGEARMAN, true);
#if defined(HAVE_LIBGEARMAN_1_0_RETURN_H) && HAVE_LIBGEARMAN_1_0_RETURN_H
  test_compare(GEARMAN_SUCCESS, GEARMAN_SUCCESS);
#endif

  return TEST_SUCCESS;
}

static test_return_t drizzled_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers and servers->validate());

#if defined(HAVE_GEARMAND_BINARY) && HAVE_GEARMAND_BINARY
  test_true(has_drizzled());
#endif

  test_skip(true, has_drizzled());

  test_skip(true, server_startup(*servers, "drizzled", get_free_port(), 0, NULL, false));

  return TEST_SUCCESS;
}

static test_return_t gearmand_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers and servers->validate());

  test_skip(true, has_gearmand());
  test_skip(true, server_startup(*servers, "gearmand", get_free_port(), 0, NULL, false));
  servers->clear();

  return TEST_SUCCESS;
}

static test_return_t skip_shim(bool a, bool b)
{
  test_skip(a, b);
  return TEST_SUCCESS;
}

static test_return_t test_skip_true_TEST(void*)
{
  test_compare(true, true);
  test_compare(false, false);
  test_compare(TEST_SUCCESS, skip_shim(true, true));
  test_compare(TEST_SUCCESS, skip_shim(false, false));

  return TEST_SUCCESS;
}

static test_return_t test_skip_false_TEST(void*)
{
  test_compare(TEST_SKIPPED, skip_shim(true, false));
  test_compare(TEST_SKIPPED, skip_shim(false, true));
  return TEST_SUCCESS;
}

static test_return_t server_startup_fail_TEST(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  fatal::disable();
  test_compare(servers->start_server(testing_service, LIBTEST_FAIL_PORT, 0, NULL, false), true);
  fatal::enable();

  return TEST_SUCCESS;
}

static test_return_t server_startup_TEST(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  test_compare(servers->start_server(testing_service, get_free_port(), 0, NULL, false), true);

  test_true(servers->last());
  pid_t last_pid= servers->last()->pid();

  test_compare(servers->last()->pid(), last_pid);
  test_true(last_pid > 1);
  test_compare(kill(last_pid, 0), 0);

  test_true(servers->shutdown());
#if 0
  test_compare(servers->last()->pid(), -1);
  test_compare(kill(last_pid, 0), -1);
#endif

  return TEST_SUCCESS;
}

static test_return_t socket_server_startup_TEST(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  test_true(servers->start_socket_server(testing_service, get_free_port(), 0, NULL, false));

  return TEST_SUCCESS;
}

#if 0
static test_return_t memcached_sasl_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  test_skip(false, bool(getenv("TESTS_ENVIRONMENT")));

  if (MEMCACHED_SASL_BINARY)
  {
    if (HAVE_LIBMEMCACHED)
    {
      test_true(has_memcached_sasl());
      test_true(server_startup(*servers, "memcached-sasl", get_free_port(), 0, NULL, false));

      return TEST_SUCCESS;
    }
  }

  return TEST_SKIPPED;
}
#endif

static test_return_t application_true_BINARY(void *)
{
  test_skip(0, access("/usr/bin/true", X_OK ));
  Application true_app("/usr/bin/true");

  test_compare(Application::SUCCESS, true_app.run());
  test_compare(Application::SUCCESS, true_app.join());

  return TEST_SUCCESS;
}

static test_return_t application_gdb_true_BINARY2(void *)
{
  test_skip(0, access("/usr/bin/gdb", X_OK ));
  test_skip(0, access("/usr/bin/true", X_OK ));

  Application true_app("/usr/bin/true");
  true_app.use_gdb();

  test_compare(Application::SUCCESS, true_app.run());
  test_compare(Application::SUCCESS, true_app.join());

  return TEST_SUCCESS;
}

static test_return_t application_gdb_true_BINARY(void *)
{
  test_skip(0, access("/usr/bin/gdb", X_OK ));
  test_skip(0, access("/usr/bin/true", X_OK ));

  Application true_app("/usr/bin/true");
  true_app.use_gdb();

  const char *args[]= { "--fubar", 0 };
  test_compare(Application::SUCCESS, true_app.run(args));
  test_compare(Application::SUCCESS, true_app.join());

  return TEST_SUCCESS;
}

static test_return_t application_true_fubar_BINARY(void *)
{
  test_skip(0, access("/usr/bin/true", X_OK ));
  Application true_app("/usr/bin/true");

  const char *args[]= { "--fubar", 0 };
  test_compare(Application::SUCCESS, true_app.run(args));
  test_compare(Application::SUCCESS, true_app.join());
  test_zero(true_app.stdout_result().size());

  return TEST_SUCCESS;
}

static test_return_t application_doesnotexist_BINARY(void *)
{

  test_skip_valgrind();
  Application true_app("doesnotexist");
  true_app.will_fail();

  const char *args[]= { "--fubar", 0 };
#if defined(TARGET_OS_OSX) && TARGET_OS_OSX
  test_compare(Application::INVALID_POSIX_SPAWN, true_app.run(args));
#elif defined(TARGET_OS_FREEBSD) && TARGET_OS_FREEBSD
  test_compare(Application::INVALID_POSIX_SPAWN, true_app.run(args));
#else
  test_compare(Application::SUCCESS, true_app.run(args));
  test_compare(Application::INVALID_POSIX_SPAWN, true_app.join());
#endif

  test_zero(true_app.stdout_result().size());

  return TEST_SUCCESS;
}

static test_return_t GET_TEST(void *)
{
  libtest::http::GET get("http://foo.example.com/");

  test_compare(false, get.execute());

  return TEST_SUCCESS;
}

static test_return_t POST_TEST(void *)
{
  libtest::vchar_t body;
  libtest::http::POST post("http://foo.example.com/", body);

  test_compare(false, post.execute());

  return TEST_SUCCESS;
}

static test_return_t TRACE_TEST(void *)
{
  libtest::vchar_t body;
  libtest::http::TRACE trace("http://foo.example.com/", body);

  test_compare(false, trace.execute());

  return TEST_SUCCESS;
}


static test_return_t vchar_t_TEST(void *)
{
  libtest::vchar_t response;
  libtest::make_vector(response, test_literal_param("fubar\n"));
  test_compare(response, response);

  return TEST_SUCCESS;
}

static test_return_t vchar_t_compare_neg_TEST(void *)
{
  libtest::vchar_t response;
  libtest::vchar_t response2;
  libtest::make_vector(response, test_literal_param("fubar\n"));
  libtest::make_vector(response2, test_literal_param(__func__));
  test_true(response != response2);

  return TEST_SUCCESS;
}

static test_return_t application_echo_fubar_BINARY(void *)
{
  if (0)
  {
    test_skip(0, access("/bin/echo", X_OK ));
    Application true_app("/bin/echo");

    const char *args[]= { "fubar", 0 };
    test_compare(Application::SUCCESS, true_app.run(args));

    while (true_app.slurp() == false) {} ;

    libtest::vchar_t response;
    make_vector(response, test_literal_param("fubar\n"));
    test_compare(response, true_app.stdout_result());
  }

  return TEST_SUCCESS;
}

static test_return_t application_echo_fubar_BINARY2(void *)
{
  if (0)
  {
    test_skip(0, access("/bin/echo", X_OK ));
    Application true_app("/bin/echo");

    true_app.add_option("fubar");

    test_compare(Application::SUCCESS, true_app.run());
    test_compare(Application::SUCCESS, true_app.join());

    libtest::vchar_t response;
    make_vector(response, test_literal_param("fubar\n"));
    test_compare(response, true_app.stdout_result());
  }

  return TEST_SUCCESS;
}

static test_return_t echo_fubar_BINARY(void *)
{
  const char *args[]= { "fubar", 0 };
  test_compare(EXIT_SUCCESS, exec_cmdline("/bin/echo", args));

  return TEST_SUCCESS;
}

static test_return_t core_count_BINARY(void *)
{
  const char *args[]= { 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline("libtest/core-count", args, true));

  return TEST_SUCCESS;
}

static test_return_t wait_BINARY(void *)
{
  const char *args[]= { "--quiet", 0 };

  test_compare(EXIT_FAILURE, exec_cmdline("libtest/wait", args, true));

  return TEST_SUCCESS;
}

static test_return_t wait_help_BINARY(void *)
{
  const char *args[]= { "--quiet", "--help", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline("libtest/wait", args, true));

  return TEST_SUCCESS;
}

static test_return_t wait_version_BINARY(void *)
{
  const char *args[]= { "--quiet", "--version", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline("libtest/wait", args, true));

  return TEST_SUCCESS;
}

static test_return_t wait_services_BINARY(void *)
{
  test_skip(0, access("/etc/services", R_OK ));

  const char *args[]= { "--quiet", "/etc/services", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline("libtest/wait", args, true));

  return TEST_SUCCESS;
}

static test_return_t wait_services_BINARY2(void *)
{
  test_skip(0, access("/etc/services", R_OK ));

  const char *args[]= { "/etc/services", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline("libtest/wait", args, true));

  return TEST_SUCCESS;
}

static test_return_t wait_services_appliction_TEST(void *)
{
  test_skip(0, access("/etc/services", R_OK ));
  test_skip(0, access("/usr/bin/gdb", X_OK ));
  test_skip(0, access("libtest/wait", X_OK ));

  libtest::Application wait_app("libtest/wait", true);
  wait_app.use_gdb();

  const char *args[]= { "/etc/services", 0 };
  test_compare(Application::SUCCESS, wait_app.run(args));
  test_compare(Application::SUCCESS, wait_app.join());

  return TEST_SUCCESS;
}

static test_return_t gdb_wait_services_appliction_TEST(void *)
{
  test_skip(true, false);
#if defined(TARGET_OS_OSX) && TARGET_OS_OSX
  test_skip(0, TARGET_OS_OSX);
#endif

  test_skip(0, access("/etc/services", R_OK ));
  test_skip(0, access("/usr/bin/gdb", X_OK ));
  test_skip(0, access("libtest/wait", X_OK ));

  libtest::Application wait_app("libtest/wait", true);
  wait_app.use_gdb();

  const char *args[]= { "/etc/services", 0 };
  test_compare(Application::SUCCESS, wait_app.run(args));
  test_compare(Application::SUCCESS, wait_app.join());

  return TEST_SUCCESS;
}

static test_return_t gdb_abort_services_appliction_TEST(void *)
{
  test_skip(0, access("/usr/bin/gdb", X_OK ));
  test_skip(0, access("libtest/abort", X_OK ));
  test_skip(true, false);

#if defined(TARGET_OS_OSX) && TARGET_OS_OSX
  test_skip(0, TARGET_OS_OSX);
#endif

  libtest::Application abort_app("libtest/abort", true);
  abort_app.use_gdb();

  test_compare(Application::SUCCESS, abort_app.run());
  test_compare(Application::SUCCESS, abort_app.join());

  std::string gdb_filename= abort_app.gdb_filename();
  test_skip(0, access(gdb_filename.c_str(), R_OK ));
  const char *args[]= { "SIGABRT", gdb_filename.c_str(), 0 };
  test_compare(EXIT_SUCCESS, exec_cmdline("grep", args));

  // Sanity test
  args[0]= "THIS_WILL_NOT_BE_FOUND";
  test_compare(EXIT_FAILURE, exec_cmdline("grep", args));

  return TEST_SUCCESS;
}

static test_return_t get_free_port_TEST(void *)
{
  in_port_t ret_port;
  test_true((ret_port= get_free_port()));
  test_true(get_free_port() != default_port());
  test_true(get_free_port() != get_free_port());

  return TEST_SUCCESS;
}

static test_return_t fatal_TEST(void *)
{
  test_compare(fatal_calls++, fatal::disabled_counter());
  throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "Testing va_args based fatal(): %d", 10); 

  return TEST_SUCCESS;
}

static test_return_t number_of_cpus_TEST(void *)
{
  test_true(number_of_cpus() >= 1);

  return TEST_SUCCESS;
}

static test_return_t check_dns_TEST(void *)
{
  test_warn(libtest::check_dns(), "Broken DNS server/no DNS server found");

  return TEST_SUCCESS;
}

static test_return_t Timer_TEST(void *)
{
  int64_t minutes= random() % 50;
  minutes++;

  Timer check;

  check.reset();
  check.offset(minutes, 2, 200);

  test_compare(check.minutes(), minutes);

  return TEST_SUCCESS;
}

static test_return_t lookup_true_TEST(void *)
{
  test_warn(libtest::lookup("exist.gearman.info"), "dns is not currently working");
  return TEST_SUCCESS;
}

static test_return_t lookup_false_TEST(void *)
{
  if (libtest::lookup("does_not_exist.gearman.info"))
  {
    Error << "Broken DNS server detected";
    return TEST_SKIPPED;
  }

  return TEST_SUCCESS;
}

static test_return_t create_tmpfile_TEST(void *)
{
  test_skip(0, access("/usr/bin/touch", X_OK ));
  std::string tmp= create_tmpfile(__func__);
  test_compare(-1, access(tmp.c_str(), R_OK));
  test_compare(-1, access(tmp.c_str(), F_OK));

  Application touch_app("/usr/bin/touch");
  const char *args[]= { tmp.c_str(), 0 };
  test_compare(Application::SUCCESS, touch_app.run(args));
  test_compare(Application::SUCCESS, touch_app.join());

  test_compare(0, access(tmp.c_str(), R_OK));
  test_compare(0, unlink(tmp.c_str()));

  return TEST_SUCCESS;
}

static test_return_t fatal_message_TEST(void *)
{
  test_compare(fatal_calls++, fatal::disabled_counter());
  fatal_message("Fatal test");

  return TEST_SUCCESS;
}

static test_return_t default_port_TEST(void *)
{
  in_port_t ret_port= default_port();
  test_compare(ret_port, libtest::default_port());
  test_compare(ret_port, libtest::default_port());

  return TEST_SUCCESS;
}

static test_return_t check_for_gearman(void *)
{
  test_skip(true, HAVE_LIBGEARMAN);
  test_skip(true, has_gearmand());
#if defined(HAVE_GEARMAND_BINARY) && HAVE_GEARMAND_BINARY
  if (GEARMAND_BINARY)
  {
    test_zero(access(GEARMAND_BINARY, X_OK ));
  }
  else
  {
    return TEST_SKIPPED;
  }
#endif

  testing_service= "gearmand";

  return TEST_SUCCESS;
}

static test_return_t check_for_drizzle(void *)
{
  test_skip(true, HAVE_LIBDRIZZLE);
  test_skip(true, has_drizzled());

  testing_service= "drizzled";

  return TEST_SUCCESS;
}


test_st drizzled_tests[] ={
  {"drizzled startup-shutdown", 0, drizzled_cycle_test },
  {0, 0, 0}
};

test_st gearmand_tests[] ={
#if 0
  {"pause", 0, pause_test },
#endif
  {"gearmand startup-shutdown", 0, gearmand_cycle_test },
  {"_compare(gearman_return_t)", 0, _compare_gearman_return_t_test },
  {"server_startup(fail)", 0, server_startup_fail_TEST },
  {0, 0, 0}
};

static test_return_t clear_servers(void* object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);
  servers->clear();

  testing_service.clear();

  return TEST_SUCCESS;
}

static test_return_t check_for_libmemcached(void* object)
{
  test_skip(true, HAVE_LIBMEMCACHED);
  test_skip(true, has_memcached());

  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);
  servers->clear();

  testing_service= "memcached";

  return TEST_SUCCESS;
}

test_st memcached_TESTS[] ={
  {"memcached startup-shutdown", 0, server_startup_TEST },
  {"memcached(socket file) startup-shutdown", 0, socket_server_startup_TEST },
  {"_compare(memcached_return_t)", 0, _compare_memcached_return_t_test },
  {"server_startup(fail)", 0, server_startup_fail_TEST },
  {0, 0, 0}
};

test_st test_skip_TESTS[] ={
  {"true, true", 0, test_skip_true_TEST },
  {"true, false", 0, test_skip_false_TEST },
  {0, 0, 0}
};

test_st environment_tests[] ={
  {"getenv()", 0, getenv_TEST },
  {"LIBTOOL_COMMAND", 0, LIBTOOL_COMMAND_test },
  {"VALGRIND_COMMAND", 0, VALGRIND_COMMAND_test },
  {"HELGRIND_COMMAND", 0, HELGRIND_COMMAND_test },
  {"GDB_COMMAND", 0, GDB_COMMAND_test },
  {0, 0, 0}
};

test_st tests_log[] ={
  {"TEST_SUCCESS", false, test_success_test },
  {"TEST_FAILURE", false, test_failure_test },
  {"TEST_SUCCESS == 0", false, test_success_equals_one_test },
  {"SUCCESS", false, test_throw_success_TEST },
  {"SKIP", false, test_throw_skip_TEST },
  {"FAIL", false, test_throw_fail_TEST },
  {0, 0, 0}
};

test_st local_log[] ={
  {"test_is_local()", 0, local_test },
  {"test_is_local(NOT)", 0, local_not_test },
  {0, 0, 0}
};

test_st directories_tests[] ={
  {"var exists", 0, var_exists_test },
  {"var/tmp exists", 0, var_tmp_exists_test },
  {"var/run exists", 0, var_run_exists_test },
  {"var/log exists", 0, var_log_exists_test },
  {"var/drizzle exists", 0, var_drizzle_exists_test },
  {"var/tmp", 0, var_tmp_test },
  {"var/run", 0, var_run_test },
  {"var/log", 0, var_log_test },
  {"var/drizzle", 0, var_drizzle_test },
  {"var/tmp rm", 0, var_tmp_rm_test },
  {"var/run rm", 0, var_run_rm_test },
  {"var/log rm", 0, var_log_rm_test },
  {"var/drizzle rm", 0, var_drizzle_rm_test },
  {0, 0, 0}
};

test_st comparison_tests[] ={
  {"_compare(test_return_t)", 0, _compare_test_return_t_test },
  {0, 0, 0}
};

test_st cmdline_tests[] ={
  {"echo fubar", 0, echo_fubar_BINARY },
  {"core-count", 0, core_count_BINARY },
  {"wait --quiet", 0, wait_BINARY },
  {"wait --quiet --help", 0, wait_help_BINARY },
  {"wait --quiet --version", 0, wait_version_BINARY },
  {"wait --quiet /etc/services", 0, wait_services_BINARY },
  {"wait /etc/services", 0, wait_services_BINARY2 },
  {"wait /etc/services", 0, wait_services_appliction_TEST },
  {"gdb wait /etc/services", 0, gdb_wait_services_appliction_TEST },
  {"gdb abort", 0, gdb_abort_services_appliction_TEST },
  {0, 0, 0}
};

test_st get_free_port_TESTS[] ={
  {"get_free_port()", 0, get_free_port_TEST },
  {"default_port()", 0, default_port_TEST },
  {0, 0, 0}
};

test_st fatal_message_TESTS[] ={
  {"libtest::fatal", 0, fatal_TEST },
  {"fatal_message()", 0, fatal_message_TEST },
  {0, 0, 0}
};

test_st number_of_cpus_TESTS[] ={
  {"libtest::number_of_cpus()", 0, number_of_cpus_TEST },
  {0, 0, 0}
};

test_st create_tmpfile_TESTS[] ={
  {"libtest::create_tmpfile()", 0, create_tmpfile_TEST },
  {0, 0, 0}
};

test_st timer_TESTS[] ={
  {"libtest::Timer", 0, Timer_TEST },
  {0, 0, 0}
};

test_st dns_TESTS[] ={
  {"libtest::lookup(true)", 0, lookup_true_TEST },
  {"libtest::lookup(false)", 0, lookup_false_TEST },
  {"libtest::check_dns()", 0, check_dns_TEST },
  {0, 0, 0}
};

test_st application_tests[] ={
  {"vchar_t", 0, vchar_t_TEST },
  {"vchar_t compare()", 0, vchar_t_compare_neg_TEST },
  {"true", 0, application_true_BINARY },
  {"gbd true --fubar", 0, application_gdb_true_BINARY },
  {"gbd true", 0, application_gdb_true_BINARY2 },
  {"true --fubar", 0, application_true_fubar_BINARY },
  {"doesnotexist --fubar", 0, application_doesnotexist_BINARY },
  {"echo fubar", 0, application_echo_fubar_BINARY },
  {"echo fubar (as option)", 0, application_echo_fubar_BINARY2 },
  {0, 0, 0}
};

static test_return_t check_for_curl(void *)
{
  test_skip(true, HAVE_LIBCURL);
  return TEST_SUCCESS;
}

static test_return_t disable_fatal_exception(void *)
{
  fatal_calls= 0;
  fatal::disable();
  return TEST_SUCCESS;
}

static test_return_t enable_fatal_exception(void *)
{
  fatal::enable();
  return TEST_SUCCESS;
}

test_st http_tests[] ={
  {"GET", 0, GET_TEST },
  {"POST", 0, POST_TEST },
  {"TRACE", 0, TRACE_TEST },
  {0, 0, 0}
};

collection_st collection[] ={
  {"environment", 0, 0, environment_tests},
  {"return values", 0, 0, tests_log},
  {"test_skip()", 0, 0, test_skip_TESTS },
  {"local", 0, 0, local_log},
  {"directories", 0, 0, directories_tests},
  {"comparison", 0, 0, comparison_tests},
  {"gearmand", check_for_gearman, clear_servers, gearmand_tests},
  {"memcached", check_for_libmemcached, clear_servers, memcached_TESTS },
  {"drizzled", check_for_drizzle, clear_servers, drizzled_tests},
  {"cmdline", 0, 0, cmdline_tests},
  {"application", 0, 0, application_tests},
  {"http", check_for_curl, 0, http_tests},
  {"http", check_for_curl, 0, http_tests},
  {"get_free_port()", 0, 0, get_free_port_TESTS },
  {"fatal", disable_fatal_exception, enable_fatal_exception, fatal_message_TESTS },
  {"number_of_cpus()", 0, 0, number_of_cpus_TESTS },
  {"create_tmpfile()", 0, 0, create_tmpfile_TESTS },
  {"dns", 0, 0, dns_TESTS },
  {"libtest::Timer", 0, 0, timer_TESTS },
  {0, 0, 0, 0}
};

static void *world_create(server_startup_st& servers, test_return_t&)
{
  return &servers;
}

void get_world(libtest::Framework *world)
{
  world->collections(collection);
  world->create(world_create);
}
