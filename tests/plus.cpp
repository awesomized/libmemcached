/*
  C++ interface test
*/
#include "libmemcached/memcached.hh"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "server.h"

#include "test.h"

#include <string>

using namespace std;

extern "C" {
   void *world_create(void);
   void world_destroy(void *p);
}

static test_return basic_test(memcached_st *memc)
{
  Memcached foo(memc);
  const string value_set("This is some data");
  string value;
  size_t value_length;

  foo.set("mine", value_set, 0, 0);
  value= foo.get("mine", &value_length);

  assert((memcmp(value.c_str(), value_set.c_str(), value_length) == 0));

  return TEST_SUCCESS;
}

static test_return increment_test(memcached_st *memc)
{
  Memcached mcach(memc);
  bool rc;
  const string key("inctest");
  const string inc_value("1");
  string ret_value;
  uint64_t int_inc_value;
  uint64_t int_ret_value;
  size_t value_length;

  mcach.set(key, inc_value, 0, 0);
  ret_value= mcach.get(key, &value_length);
  printf("\nretvalue %s\n",ret_value.c_str());
  int_inc_value= uint64_t(atol(inc_value.c_str()));
  int_ret_value= uint64_t(atol(ret_value.c_str()));
  assert(int_ret_value == int_inc_value); 

  rc= mcach.increment(key, 1, &int_ret_value);
  assert(rc == true);
  assert(int_ret_value == 2);

  rc= mcach.increment(key, 1, &int_ret_value);
  assert(rc == true);
  assert(int_ret_value == 3);

  rc= mcach.increment(key, 5, &int_ret_value);
  assert(rc == true);
  assert(int_ret_value == 8);

  return TEST_SUCCESS;
}

static test_return basic_master_key_test(memcached_st *memc)
{
  Memcached foo(memc);
  const string value_set("Data for server A");
  const string master_key_a("server-a");
  const string master_key_b("server-b");
  const string key("xyz");
  string value;
  size_t value_length;

  foo.set_by_key(master_key_a, key, value_set, 0, 0);
  value= foo.get_by_key(master_key_a, key, &value_length);

  assert((memcmp(value.c_str(), value_set.c_str(), value_length) == 0));

  value= foo.get_by_key(master_key_b, key, &value_length);
  assert((memcmp(value.c_str(), value_set.c_str(), value_length) == 0));

  return TEST_SUCCESS;
}

/* Count the results */
static memcached_return callback_counter(memcached_st *ptr __attribute__((unused)), 
                                     memcached_result_st *result __attribute__((unused)), 
                                     void *context)
{
  unsigned int *counter= static_cast<unsigned int *>(context);

  *counter= *counter + 1;

  return MEMCACHED_SUCCESS;
}

static test_return mget_result_function(memcached_st *memc)
{
  Memcached mc(memc);
  bool rc;
  string key1("fudge");
  string key2("son");
  string key3("food");
  vector<string> keys;
  keys.reserve(3);
  keys.push_back(key1);
  keys.push_back(key2);
  keys.push_back(key3);
  unsigned int counter;
  memcached_execute_function callbacks[1];

  /* We need to empty the server before we continue the test */
  rc= mc.flush(0);
  rc= mc.set_all(keys, keys, 50, 9);
  assert(rc == true);

  rc= mc.mget(keys);
  assert(rc == true);

  callbacks[0]= &callback_counter;
  counter= 0;
  rc= mc.fetch_execute(callbacks, static_cast<void *>(&counter), 1); 

  assert(counter == 3);

  return TEST_SUCCESS;
}

static test_return mget_test(memcached_st *memc)
{
  Memcached mc(memc);
  bool rc;
  memcached_return mc_rc;
  vector<string> keys;
  keys.reserve(3);
  keys.push_back("fudge");
  keys.push_back("son");
  keys.push_back("food");
  uint32_t flags;

  string return_key;
  size_t return_key_length;
  string return_value;
  size_t return_value_length;

  /* We need to empty the server before we continue the test */
  rc= mc.flush(0);
  assert(rc == true);

  rc= mc.mget(keys);
  assert(rc == true);

  while (mc.fetch(return_key, return_value, &return_key_length, 
                  &return_value_length, &flags, &mc_rc))
  {
    assert(return_value.length() != 0);
  }
  assert(return_value_length == 0);
  assert(mc_rc == MEMCACHED_END);

  rc= mc.set_all(keys, keys, 50, 9);
  assert(rc == true);

  rc= mc.mget(keys);
  assert(rc == true);

  while ((mc.fetch(return_key, return_value, &return_key_length, 
                   &return_value_length, &flags, &mc_rc)))
  {
    assert(return_value.length() != 0);
    assert(mc_rc == MEMCACHED_SUCCESS);
    assert(return_key_length == return_value_length);
    assert(!memcmp(return_value.c_str(), return_key.c_str(), return_value_length));
  }

  return TEST_SUCCESS;
}

test_st tests[] ={
  { "basic", 0, basic_test },
  { "basic_master_key", 0, basic_master_key_test },
  { "increment_test", 0, increment_test },
  { "mget", 1, mget_test },
  { "mget_result_function", 1, mget_result_function },
  {0, 0, 0}
};

collection_st collection[] ={
  {"block", 0, 0, tests},
  {0, 0, 0, 0}
};

#define SERVERS_TO_CREATE 1

extern "C" void *world_create(void)
{
  server_startup_st *construct;

  construct= (server_startup_st *)malloc(sizeof(server_startup_st));
  memset(construct, 0, sizeof(server_startup_st));

  construct->count= SERVERS_TO_CREATE;
  server_startup(construct);

  return construct;
}

void world_destroy(void *p)
{
  server_startup_st *construct= static_cast<server_startup_st *>(p);
  memcached_server_st *servers=
    static_cast<memcached_server_st *>(construct->servers);
  memcached_server_list_free(servers);

  server_shutdown(construct);
  free(construct);
}

void get_world(world_st *world)
{
  world->collections= collection;
  world->create= world_create;
  world->destroy= world_destroy;
}
