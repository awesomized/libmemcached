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

#include <libtest/test.hpp>

#if defined(LIBTEST_WITH_LIBMEMCACHED_SUPPORT) && LIBTEST_WITH_LIBMEMCACHED_SUPPORT
#include <libmemcached-1.0/memcached.h>
#endif

#if defined(LIBTEST_WITH_LIBGEARMAN_SUPPORT) && LIBTEST_WITH_LIBGEARMAN_SUPPORT
#include <libgearman/gearman.h>
#endif

#include <cstdlib>
#include <unistd.h>

using namespace libtest;

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
  test_skip(HAVE_LIBMEMCACHED, true);
#if defined(HAVE_LIBMEMCACHED) && HAVE_LIBMEMCACHED 
  test_zero(MEMCACHED_SUCCESS);
#endif
  return TEST_SUCCESS;
}

static test_return_t test_success_test(void *)
{
  return TEST_SUCCESS;
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
  _compare(__FILE__, __LINE__, __func__, 0, unsetenv("LIBTEST_LOCAL"));
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

static test_return_t var_tmp_test(void *)
{
  FILE *file= fopen("var/tmp/junk", "w+");
  char buffer[1024];
  const char *dir= getcwd(buffer, sizeof(buffer));
  test_true_got(file, dir);
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

static test_return_t _compare_test_return_t_test(void *)
{
  test_compare(TEST_SUCCESS, TEST_SUCCESS);

  return TEST_SUCCESS;
}

static test_return_t _compare_memcached_return_t_test(void *)
{
  test_skip(HAVE_LIBMEMCACHED, true);
#if defined(HAVE_LIBMEMCACHED) && HAVE_LIBMEMCACHED 
  test_compare(MEMCACHED_SUCCESS, MEMCACHED_SUCCESS);
#endif

  return TEST_SUCCESS;
}

static test_return_t _compare_gearman_return_t_test(void *)
{
  test_skip(HAVE_LIBGEARMAN, true);
#if defined(HAVE_LIBGEARMAN) && HAVE_LIBGEARMAN
  test_compare(GEARMAN_SUCCESS, GEARMAN_SUCCESS);
#endif

  return TEST_SUCCESS;
}

static test_return_t gearmand_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

#if defined(HAVE_GEARMAND_BINARY) && HAVE_GEARMAND_BINARY
  test_true(has_gearmand_binary());
#else
  test_skip(true, has_gearmand_binary());
#endif

  const char *argv[1]= { "cycle_gearmand" };
  test_true(server_startup(*servers, "gearmand", get_free_port(), 1, argv));

  return TEST_SUCCESS;
}

static test_return_t memcached_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  if (MEMCACHED_BINARY and HAVE_LIBMEMCACHED) 
  {
    test_true(has_memcached_binary());
    const char *argv[1]= { "cycle_memcached" };
    test_true(server_startup(*servers, "memcached", get_free_port(), 1, argv));

    return TEST_SUCCESS;
  }

  return TEST_SKIPPED;
}

static test_return_t memcached_socket_cycle_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  if (MEMCACHED_BINARY)
  {
    if (HAVE_LIBMEMCACHED)
    {
      test_true(has_memcached_binary());
      const char *argv[1]= { "cycle_memcached" };
      test_true(servers->start_socket_server("memcached", get_free_port(), 1, argv));

      return TEST_SUCCESS;
    }
  }

  return TEST_SKIPPED;
}

static test_return_t memcached_sasl_test(void *object)
{
  server_startup_st *servers= (server_startup_st*)object;
  test_true(servers);

  if (getenv("TESTS_ENVIRONMENT"))
  {
    return TEST_SKIPPED;
  }

  if (MEMCACHED_SASL_BINARY)
  {
    if (HAVE_LIBMEMCACHED)
    {
      test_true(has_memcached_sasl_binary());
      const char *argv[1]= { "cycle_memcached_sasl" };
      test_true(server_startup(*servers, "memcached-sasl", get_free_port(), 1, argv));

      return TEST_SUCCESS;
    }
  }

  return TEST_SKIPPED;
}

static test_return_t application_true_BINARY(void *)
{
  Application true_app("true");

  test_compare(Application::SUCCESS, true_app.run());
  test_compare(Application::SUCCESS, true_app.wait());

  return TEST_SUCCESS;
}

static test_return_t application_true_fubar_BINARY(void *)
{
  Application true_app("true");

  const char *args[]= { "--fubar", 0 };
  test_compare(Application::SUCCESS, true_app.run(args));
  test_compare(Application::SUCCESS, true_app.wait());
  test_compare(0, true_app.stdout_result().size());

  return TEST_SUCCESS;
}

static test_return_t application_true_fubar_eq_doh_BINARY(void *)
{
  Application true_app("true");

  const char *args[]= { "--fubar=doh", 0 };
  test_compare(Application::SUCCESS, true_app.run(args));
  test_compare(Application::SUCCESS, true_app.wait());
  test_compare(0, true_app.stdout_result().size());

  return TEST_SUCCESS;
}

static test_return_t application_true_fubar_eq_doh_option_BINARY(void *)
{
  Application true_app("true");

  true_app.add_option("--fubar=", "doh");

  test_compare(Application::SUCCESS, true_app.run());
  test_compare(Application::SUCCESS, true_app.wait());
  test_compare(0, true_app.stdout_result().size());

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

static test_return_t application_echo_fubar_BINARY(void *)
{
  Application true_app("echo");

  const char *args[]= { "fubar", 0 };
  test_compare(Application::SUCCESS, true_app.run(args));
  test_compare(Application::SUCCESS, true_app.wait());

  libtest::vchar_t response;
  make_vector(response, test_literal_param("fubar\n"));
  test_compare(response, true_app.stdout_result());

  return TEST_SUCCESS;
}

static test_return_t application_echo_fubar_BINARY2(void *)
{
  Application true_app("echo");

  true_app.add_option("fubar");

  test_compare(Application::SUCCESS, true_app.run());
  test_compare(Application::SUCCESS, true_app.wait());
  libtest::vchar_t response;
  make_vector(response, test_literal_param("fubar\n"));
  test_compare(response, true_app.stdout_result());

  return TEST_SUCCESS;
}

static test_return_t true_BINARY(void *)
{
  const char *args[]= { 0 };
  test_compare(EXIT_SUCCESS, exec_cmdline("true", args));

  return TEST_SUCCESS;
}

static test_return_t true_fubar_BINARY(void *)
{
  const char *args[]= { "--fubar", 0 };
  test_compare(EXIT_SUCCESS, exec_cmdline("true", args));

  return TEST_SUCCESS;
}

static test_return_t echo_fubar_BINARY(void *)
{
  const char *args[]= { "fubar", 0 };
  test_compare(EXIT_SUCCESS, exec_cmdline("echo", args));

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

static test_return_t application_wait_services_BINARY2(void *)
{
  test_skip(0, access("/etc/services", R_OK ));

  libtest::Application("libtest/wait", true);
  const char *args[]= { "/etc/services", 0 };

  test_compare(EXIT_SUCCESS, exec_cmdline("libtest/wait", args, true));

  return TEST_SUCCESS;
}

static test_return_t get_free_port_TEST(void *)
{
  in_port_t ret_port;
  test_true_hint((ret_port= get_free_port()), ret_port);
  test_true(get_free_port() != default_port());
  test_true(get_free_port() != get_free_port());

  return TEST_SUCCESS;
}

static uint32_t fatal_calls= 0;

static test_return_t fatal_TEST(void *)
{
  test_compare(fatal_calls++, fatal::disabled_counter());
  throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "Testing va_args based fatal(): %d", 10); 

  return TEST_SUCCESS;
}

static test_return_t fatal_message_TEST(void *)
{
  test_compare(fatal_calls++, fatal::disabled_counter());
  throw fatal_message("Fatal test");

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
  test_skip(true, has_gearmand_binary());
  return TEST_SUCCESS;
}


test_st gearmand_tests[] ={
#if 0
  {"pause", 0, pause_test },
#endif
  {"gearmand startup-shutdown", 0, gearmand_cycle_test },
  {"_compare(gearman_return_t)", 0, _compare_gearman_return_t_test },
  {0, 0, 0}
};

static test_return_t check_for_libmemcached(void *)
{
  test_skip(true, HAVE_LIBMEMCACHED);
  test_skip(true, has_memcached_binary());
  return TEST_SUCCESS;
}

test_st memcached_tests[] ={
  {"memcached startup-shutdown", 0, memcached_cycle_test },
  {"memcached(socket file) startup-shutdown", 0, memcached_socket_cycle_test },
  {"memcached_sasl() startup-shutdown", 0, memcached_sasl_test },
  {"_compare(memcached_return_t)", 0, _compare_memcached_return_t_test },
  {0, 0, 0}
};

test_st environment_tests[] ={
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
  {"var/tmp", 0, var_tmp_test },
  {"var/run", 0, var_run_test },
  {"var/log", 0, var_log_test },
  {"var/tmp rm", 0, var_tmp_rm_test },
  {"var/run rm", 0, var_run_rm_test },
  {"var/log rm", 0, var_log_rm_test },
  {0, 0, 0}
};

test_st comparison_tests[] ={
  {"_compare(test_return_t)", 0, _compare_test_return_t_test },
  {0, 0, 0}
};

test_st cmdline_tests[] ={
  {"true", 0, true_BINARY },
  {"true --fubar", 0, true_fubar_BINARY },
  {"echo fubar", 0, echo_fubar_BINARY },
  {"wait --quiet", 0, wait_BINARY },
  {"wait --quiet --help", 0, wait_help_BINARY },
  {"wait --quiet --version", 0, wait_version_BINARY },
  {"wait --quiet /etc/services", 0, wait_services_BINARY },
  {"wait /etc/services", 0, wait_services_BINARY2 },
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

test_st application_tests[] ={
  {"vchar_t", 0, vchar_t_TEST },
  {"true", 0, application_true_BINARY },
  {"true --fubar", 0, application_true_fubar_BINARY },
  {"true --fubar=doh", 0, application_true_fubar_eq_doh_BINARY },
  {"true --fubar=doh add_option()", 0, application_true_fubar_eq_doh_option_BINARY },
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
  fatal::disable();
  return TEST_SUCCESS;
}

static test_return_t enable_fatal_exception(void *)
{
  fatal::disable();
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
  {"local", 0, 0, local_log},
  {"directories", 0, 0, directories_tests},
  {"comparison", 0, 0, comparison_tests},
  {"gearmand", check_for_gearman, 0, gearmand_tests},
  {"memcached", check_for_libmemcached, 0, memcached_tests},
  {"cmdline", 0, 0, cmdline_tests},
  {"application", 0, 0, application_tests},
  {"http", check_for_curl, 0, http_tests},
  {"get_free_port()", 0, 0, get_free_port_TESTS },
  {"fatal", disable_fatal_exception, enable_fatal_exception, fatal_message_TESTS },
  {0, 0, 0, 0}
};

static void *world_create(server_startup_st& servers, test_return_t&)
{
  return &servers;
}

void get_world(Framework *world)
{
  world->collections= collection;
  world->_create= world_create;
}
