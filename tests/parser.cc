/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached Client and Server 
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  All rights reserved.
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

#include <vector>
#include <iostream>
#include <string>

#define BUILDING_LIBMEMCACHED
#include <libmemcached/memcached.h>

#include "tests/parser.h"
#include "tests/print.h"

enum scanner_type_t
{
  NIL,
  UNSIGNED,
  SIGNED,
  ARRAY
};


struct scanner_string_st {
  const char *c_str;
  size_t size;
};

static inline scanner_string_st scanner_string(const char *arg, size_t arg_size)
{
  scanner_string_st local= { arg, arg_size };
  return local;
}

#define make_scanner_string(X) scanner_string((X), static_cast<size_t>(sizeof(X) - 1))

static struct scanner_string_st scanner_string_null= { 0, 0};

struct scanner_variable_t {
  enum scanner_type_t type;
  struct scanner_string_st option;
  struct scanner_string_st result;
  test_return_t (*check_func)(memcached_st *memc, const scanner_string_st &hostname);
};

// Check and make sure the first host is what we expect it to be
static test_return_t __check_host(memcached_st *memc, const scanner_string_st &hostname)
{
  memcached_server_instance_st instance=
    memcached_server_instance_by_position(memc, 0);

  test_true(instance);

  const char *first_hostname = memcached_server_name(instance);
  test_true(first_hostname);
  test_strcmp(first_hostname, hostname.c_str);

  return TEST_SUCCESS;
}

// Check and make sure the prefix_key is what we expect it to be
static test_return_t __check_prefix_key(memcached_st *memc, const scanner_string_st &hostname)
{
  memcached_server_instance_st instance=
    memcached_server_instance_by_position(memc, 0);

  test_true(instance);

  const char *first_hostname = memcached_server_name(instance);
  test_true(first_hostname);
  test_strcmp(first_hostname, hostname.c_str);

  return TEST_SUCCESS;
}

static test_return_t __check_IO_MSG_WATERMARK(memcached_st *memc, const scanner_string_st &value)
{
  uint64_t value_number;

  value_number= atoll(value.c_str);

  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK) == value_number);
  return TEST_SUCCESS;
}

static test_return_t __check_AUTO_EJECT_HOSTS(memcached_st *memc, const scanner_string_st &value)
{
  (void)value;
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS));
  return TEST_SUCCESS;
}

static test_return_t __check_NOREPLY(memcached_st *memc, const scanner_string_st &value)
{
  (void)value;
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_NOREPLY));
  return TEST_SUCCESS;
}

static test_return_t __check_VERIFY_KEY(memcached_st *memc, const scanner_string_st &value)
{
  (void)value;
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_VERIFY_KEY));
  return TEST_SUCCESS;
}

static test_return_t __check_distribution_RANDOM(memcached_st *memc, const scanner_string_st &value)
{
  (void)value;
  test_true(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_DISTRIBUTION) == MEMCACHED_DISTRIBUTION_RANDOM);
  return TEST_SUCCESS;
}

scanner_variable_t test_server_strings[]= {
  { ARRAY, make_scanner_string("--server=localhost"), make_scanner_string("localhost"), __check_host },
  { ARRAY, make_scanner_string("--server=10.0.2.1"), make_scanner_string("10.0.2.1"), __check_host },
  { ARRAY, make_scanner_string("--server=example.com"), make_scanner_string("example.com"), __check_host },
  { ARRAY, make_scanner_string("--server=localhost:30"), make_scanner_string("localhost"), __check_host },
  { ARRAY, make_scanner_string("--server=10.0.2.1:20"), make_scanner_string("10.0.2.1"), __check_host },
  { ARRAY, make_scanner_string("--server=example.com:1024"), make_scanner_string("example.com"), __check_host },
  { NIL, scanner_string_null, scanner_string_null, NULL }
};

scanner_variable_t test_servers_strings[]= {
  { ARRAY, make_scanner_string("--servers=localhost:11221,localhost:11222,localhost:11223,localhost:11224,localhost:11225"), scanner_string_null, NULL },
  { ARRAY, make_scanner_string("--servers=a.example.com:81,localhost:82,b.example.com"), scanner_string_null, NULL },
  { ARRAY, make_scanner_string("--servers=localhost,localhost:80"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};


scanner_variable_t bad_test_strings[]= {
  { ARRAY, make_scanner_string("-servers=localhost:11221,localhost:11222,localhost:11223,localhost:11224,localhost:11225"), scanner_string_null, NULL },
  { ARRAY, make_scanner_string("-- servers=a.example.com:81,localhost:82,b.example.com"), scanner_string_null, NULL },
  { ARRAY, make_scanner_string("--servers=localhost+80"), scanner_string_null, NULL},
  { ARRAY, make_scanner_string("--servers=localhost.com."), scanner_string_null, NULL},
  { ARRAY, make_scanner_string("--server=localhost.com."), scanner_string_null, NULL},
  { ARRAY, make_scanner_string("--server=localhost.com.:80"), scanner_string_null, NULL},
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t test_number_options[]= {
  { ARRAY,  make_scanner_string("--CONNECT_TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--IO_MSG_WATERMARK=456"), make_scanner_string("456"), __check_IO_MSG_WATERMARK },
  { ARRAY,  make_scanner_string("--IO_BYTES_WATERMARK=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--IO_KEY_PREFETCH=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--NUMBER_OF_REPLICAS=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--POLL_TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--RCV_TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--RETRY_TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SERVER_FAILURE_LIMIT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SND_TIMEOUT=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SOCKET_RECV_SIZE=456"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SOCKET_SEND_SIZE=456"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t test_boolean_options[]= {
  { ARRAY,  make_scanner_string("--AUTO_EJECT_HOSTS"), scanner_string_null, __check_AUTO_EJECT_HOSTS },
  { ARRAY,  make_scanner_string("--BINARY_PROTOCOL"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--BUFFER_REQUESTS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH_WITH_PREFIX_KEY"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--KETAMA_WEIGHTED"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--NOREPLY"), scanner_string_null, __check_NOREPLY },
  { ARRAY,  make_scanner_string("--RANDOMIZE_REPLICA_READ"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SORT_HOSTS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--SUPPORT_CAS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--TCP_NODELAY"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--TCP_KEEPALIVE"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--TCP_KEEPIDLE"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--USE_UDP"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--VERIFY_KEY"), scanner_string_null, __check_VERIFY_KEY },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t prefix_key_strings[]= {
  { ARRAY, make_scanner_string("--PREFIX_KEY=foo"), make_scanner_string("foo"), __check_prefix_key },
  { ARRAY, make_scanner_string("--PREFIX-KEY=\"foo\""), make_scanner_string("foo"), __check_prefix_key },
  { ARRAY, make_scanner_string("--PREFIX-KEY=\"This is a very long key\""), make_scanner_string("This is a very long key"), __check_prefix_key },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t distribution_strings[]= {
  { ARRAY,  make_scanner_string("--DISTRIBUTION=consistent"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=consistent,CRC"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=consistent,MD5"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=random"), scanner_string_null, __check_distribution_RANDOM },
  { ARRAY,  make_scanner_string("--DISTRIBUTION=modula"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};

scanner_variable_t hash_strings[]= {
  { ARRAY,  make_scanner_string("--HASH=CRC"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1A_32"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1A_64"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1_32"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=FNV1_64"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=JENKINS"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=MD5"), scanner_string_null, NULL },
  { ARRAY,  make_scanner_string("--HASH=MURMUR"), scanner_string_null, NULL },
  { NIL, scanner_string_null, scanner_string_null, NULL}
};


static test_return_t _test_option(scanner_variable_t *scanner, bool test_true= true)
{
  (void)test_true;
  memcached_st *memc;
  memc= memcached_create(NULL);

  for (scanner_variable_t *ptr= scanner; ptr->type != NIL; ptr++)
  {
    memcached_return_t rc;
    rc= memcached_parse_configuration(memc, ptr->option.c_str, ptr->option.size);
    if (test_true)
    {
      if (rc != MEMCACHED_SUCCESS)
        memcached_error_print(memc);

      test_true(rc == MEMCACHED_SUCCESS);

      if (ptr->check_func)
      {
        (*ptr->check_func)(memc, ptr->result);
      }
    }
    else
    {
      test_false_with(rc == MEMCACHED_SUCCESS, ptr->option.c_str);
    }
    memcached_reset(memc);
  }
  memcached_free(memc);

  return TEST_SUCCESS;
}

test_return_t server_test(memcached_st *junk)
{
  (void)junk;
  return _test_option(test_server_strings);
}

test_return_t servers_test(memcached_st *junk)
{
  (void)junk;

  test_return_t rc;
  if ((rc= _test_option(test_server_strings)) != TEST_SUCCESS)
  {
    return rc;
  }

#if 0
    memcached_server_fn callbacks[1];
    callbacks[0]= server_print_callback;
    memcached_server_cursor(memc, callbacks, NULL,  1);
#endif

  if ((rc= _test_option(bad_test_strings, false)) != TEST_SUCCESS)
  {
    return rc;
  }

  return TEST_SUCCESS;
}

test_return_t parser_number_options_test(memcached_st *junk)
{
  (void)junk;
  return _test_option(test_number_options);
}

test_return_t parser_boolean_options_test(memcached_st *junk)
{
  (void)junk;
  return _test_option(test_boolean_options);
}

test_return_t behavior_parser_test(memcached_st *junk)
{
  (void)junk;
  return TEST_SUCCESS;
}

test_return_t parser_hash_test(memcached_st *junk)
{
  (void)junk;
  return _test_option(hash_strings);
}

test_return_t parser_distribution_test(memcached_st *junk)
{
  (void)junk;
  return _test_option(distribution_strings);
}

test_return_t parser_key_prefix_test(memcached_st *junk)
{
  (void)junk;
  return _test_option(distribution_strings);
}

#define SUPPORT_EXAMPLE_CNF "support/example.cnf"

test_return_t memcached_parse_configure_file_test(memcached_st *junk)
{
  (void)junk;

  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  memcached_st memc;
  memcached_st *memc_ptr= memcached_create(&memc);

  test_true(memc_ptr);

  memcached_return_t rc= memcached_parse_configure_file(memc_ptr, memcached_string_with_size(SUPPORT_EXAMPLE_CNF));
  test_true_got(rc == MEMCACHED_SUCCESS, memcached_last_error_message(memc_ptr) ? memcached_last_error_message(memc_ptr) : memcached_strerror(NULL, rc));
  memcached_free(memc_ptr);

  return TEST_SUCCESS;
}

test_return_t memcached_create_with_options_with_filename(memcached_st *junk)
{
  (void)junk;
  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  memcached_st *memc_ptr;
  memc_ptr= memcached_create_with_options(STRING_WITH_LEN("--CONFIGURE-FILE=\"support/example.cnf\""));
  test_true_got(memc_ptr, memcached_last_error_message(memc_ptr));
  memcached_free(memc_ptr);

  return TEST_SUCCESS;
}

test_return_t libmemcached_check_configuration_with_filename_test(memcached_st *junk)
{
  (void)junk;

  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  memcached_return_t rc;
  char buffer[BUFSIZ];

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--CONFIGURE-FILE=\"support/example.cnf\""), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--CONFIGURE-FILE=support/example.cnf"), buffer, sizeof(buffer));
  test_false_with(rc == MEMCACHED_SUCCESS, buffer);

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--CONFIGURE-FILE=\"bad-path/example.cnf\""), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_ERRNO, buffer);

  return TEST_SUCCESS;
}

test_return_t libmemcached_check_configuration_test(memcached_st *junk)
{
  (void)junk;

  memcached_return_t rc;
  char buffer[BUFSIZ];

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost"), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  rc= libmemcached_check_configuration(STRING_WITH_LEN("--dude=localhost"), buffer, sizeof(buffer));
  test_false_with(rc == MEMCACHED_SUCCESS, buffer);
  test_true(rc == MEMCACHED_PARSE_ERROR);

  return TEST_SUCCESS;
}

test_return_t memcached_create_with_options_test(memcached_st *junk)
{
  (void)junk;

  memcached_st *memc_ptr;
  memc_ptr= memcached_create_with_options(STRING_WITH_LEN("--server=localhost"));
  test_true_got(memc_ptr, memcached_last_error_message(memc_ptr));
  memcached_free(memc_ptr);

  memc_ptr= memcached_create_with_options(STRING_WITH_LEN("--dude=localhost"));
  test_false_with(memc_ptr, memcached_last_error_message(memc_ptr));

  return TEST_SUCCESS;
}

test_return_t test_include_keyword(memcached_st *junk)
{
  if (access(SUPPORT_EXAMPLE_CNF, R_OK))
    return TEST_SKIPPED;

  (void)junk;
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("INCLUDE \"support/example.cnf\""), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

test_return_t test_end_keyword(memcached_st *junk)
{
  (void)junk;
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost END bad keywords"), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

test_return_t test_reset_keyword(memcached_st *junk)
{
  (void)junk;
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost reset --server=bad.com"), buffer, sizeof(buffer));
  test_true_got(rc == MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

test_return_t test_error_keyword(memcached_st *junk)
{
  (void)junk;
  char buffer[BUFSIZ];
  memcached_return_t rc;
  rc= libmemcached_check_configuration(STRING_WITH_LEN("--server=localhost ERROR --server=bad.com"), buffer, sizeof(buffer));
  test_true_got(rc != MEMCACHED_SUCCESS, buffer);

  return TEST_SUCCESS;
}

#define RANDOM_STRINGS 50
test_return_t random_statement_build_test(memcached_st *junk)
{
  (void)junk;
  std::vector<scanner_string_st *> option_list;

  for (scanner_variable_t *ptr= test_server_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

#if 0
  for (scanner_variable_t *ptr= test_servers_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);
#endif

  for (scanner_variable_t *ptr= test_number_options; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= test_boolean_options; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= prefix_key_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= distribution_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (scanner_variable_t *ptr= hash_strings; ptr->type != NIL; ptr++)
    option_list.push_back(&ptr->option);

  for (uint32_t x= 0; x < RANDOM_STRINGS; x++)
  {
    std::string random_options;

    uint32_t number_of= random() % option_list.size();
    for (uint32_t options= 0; options < number_of; options++)
    {
      random_options+= option_list[random() % option_list.size()]->c_str;
      random_options+= " ";
    }

    memcached_return_t rc;
    memcached_st *memc_ptr= memcached_create(NULL);
    rc= memcached_parse_configuration(memc_ptr, random_options.c_str(), random_options.size() -1);
    if (rc == MEMCACHED_PARSE_ERROR)
    {
      std::cerr << std::endl << "Failed to parse(" << memcached_strerror(NULL, rc) << "): " << random_options << std::endl;
      memcached_error_print(memc_ptr);
    }
    memcached_free(memc_ptr);
  }

  return TEST_SUCCESS;
}
