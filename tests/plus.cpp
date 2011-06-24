/*
  C++ interface test
*/
#include <libmemcached/memcached.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

#include <libtest/server.h>

#include <libtest/test.hpp>

#include <string>
#include <iostream>

using namespace std;
using namespace memcache;

extern "C" {
   test_return_t basic_test(memcached_st *memc);
   test_return_t increment_test(memcached_st *memc);
   test_return_t basic_master_key_test(memcached_st *memc);
   test_return_t mget_result_function(memcached_st *memc);
   test_return_t basic_behavior(memcached_st *memc);
   test_return_t mget_test(memcached_st *memc);
   memcached_return_t callback_counter(const memcached_st *,
                                       memcached_result_st *,
                                       void *context);
}

static void populate_vector(vector<char> &vec, const string &str)
{
  vec.reserve(str.length());
  vec.assign(str.begin(), str.end());
}

static void copy_vec_to_string(vector<char> &vec, string &str)
{
  str.clear();
  if (not vec.empty())
  {
    str.assign(vec.begin(), vec.end());
  }
}

test_return_t basic_test(memcached_st *memc)
{
  Memcache foo(memc);
  const string value_set("This is some data");
  std::vector<char> value;
  std::vector<char> test_value;

  populate_vector(value, value_set);

  test_true(foo.set("mine", value, 0, 0));
  test_true(foo.get("mine", test_value));

  test_memcmp(&test_value[0], &value[0], test_value.size());
  test_false(foo.set("", value, 0, 0));

  return TEST_SUCCESS;
}

test_return_t increment_test(memcached_st *original)
{
  Memcache mcach(original);
  const string key("blah");
  const string inc_value("1");
  std::vector<char> inc_val;
  vector<char> ret_value;
  string ret_string;
  uint64_t int_inc_value;
  uint64_t int_ret_value;

  populate_vector(inc_val, inc_value);

  test_true(mcach.set(key, inc_val, 0, 0));

  test_true(mcach.get(key, ret_value));
  test_false(ret_value.empty());
  copy_vec_to_string(ret_value, ret_string);

  int_inc_value= uint64_t(atol(inc_value.c_str()));
  int_ret_value= uint64_t(atol(ret_string.c_str()));
  test_compare(int_inc_value, int_ret_value);

  test_true(mcach.increment(key, 1, &int_ret_value));
  test_compare(2, int_ret_value);

  test_true(mcach.increment(key, 1, &int_ret_value));
  test_compare(3, int_ret_value);

  test_true(mcach.increment(key, 5, &int_ret_value));
  test_compare(8, int_ret_value);

  return TEST_SUCCESS;
}

test_return_t basic_master_key_test(memcached_st *original)
{
  Memcache foo(original);
  const string value_set("Data for server A");
  vector<char> value;
  vector<char> test_value;
  const string master_key_a("server-a");
  const string master_key_b("server-b");
  const string key("xyz");

  populate_vector(value, value_set);

  foo.setByKey(master_key_a, key, value, 0, 0);
  foo.getByKey(master_key_a, key, test_value);

  test_true((memcmp(&value[0], &test_value[0], value.size()) == 0));

  test_value.clear();

  foo.getByKey(master_key_b, key, test_value);
  test_true((memcmp(&value[0], &test_value[0], value.size()) == 0));

  return TEST_SUCCESS;
}

/* Count the results */
memcached_return_t callback_counter(const memcached_st *,
                                    memcached_result_st *,
                                    void *context)
{
  unsigned int *counter= static_cast<unsigned int *>(context);

  *counter= *counter + 1;

  return MEMCACHED_SUCCESS;
}

test_return_t mget_test(memcached_st *original)
{
  Memcache memc(original);
  memcached_return_t mc_rc;
  vector<string> keys;
  vector< vector<char> *> values;
  keys.reserve(3);
  keys.push_back("fudge");
  keys.push_back("son");
  keys.push_back("food");
  vector<char> val1;
  vector<char> val2;
  vector<char> val3;
  populate_vector(val1, "fudge");
  populate_vector(val2, "son");
  populate_vector(val3, "food");
  values.reserve(3);
  values.push_back(&val1);
  values.push_back(&val2);
  values.push_back(&val3);

  string return_key;
  vector<char> return_value;

  /* We need to empty the server before we continue the test */
  test_true(memc.flush());

  test_true(memc.mget(keys));

  test_compare(MEMCACHED_NOTFOUND, 
               memc.fetch(return_key, return_value));

  test_true(memc.setAll(keys, values, 50, 9));

  test_true(memc.mget(keys));
  size_t count= 0;
  while ((mc_rc= memc.fetch(return_key, return_value)) == MEMCACHED_SUCCESS)
  {
    test_compare(return_key.length(), return_value.size());
    test_memcmp(&return_value[0], return_key.c_str(), return_value.size());
    count++;
  }
  test_compare(values.size(), count);

  return TEST_SUCCESS;
}

test_return_t basic_behavior(memcached_st *original)
{
  Memcache memc(original);
  uint64_t value= 1;
  test_true(memc.setBehavior(MEMCACHED_BEHAVIOR_VERIFY_KEY, value));
  uint64_t behavior= memc.getBehavior(MEMCACHED_BEHAVIOR_VERIFY_KEY);
  test_compare(behavior, value);

  return TEST_SUCCESS;
}

test_st tests[] ={
  { "basic", 0,
    reinterpret_cast<test_callback_fn*>(basic_test) },
  { "basic_master_key", 0,
    reinterpret_cast<test_callback_fn*>(basic_master_key_test) },
  { "increment_test", 0,
    reinterpret_cast<test_callback_fn*>(increment_test) },
  { "mget", 1,
    reinterpret_cast<test_callback_fn*>(mget_test) },
  { "basic_behavior", 0,
    reinterpret_cast<test_callback_fn*>(basic_behavior) },
  {0, 0, 0}
};

collection_st collection[] ={
  {"block", 0, 0, tests},
  {0, 0, 0, 0}
};

#define SERVERS_TO_CREATE 5

#include "libmemcached_world.h"

void get_world(Framework *world)
{
  world->collections= collection;

  world->_create= reinterpret_cast<test_callback_create_fn*>(world_create);
  world->_destroy= reinterpret_cast<test_callback_fn*>(world_destroy);

  world->item._startup= reinterpret_cast<test_callback_fn*>(world_test_startup);
  world->item._flush= reinterpret_cast<test_callback_fn*>(world_flush);
  world->item.set_pre(reinterpret_cast<test_callback_fn*>(world_pre_run));
  world->item.set_post(reinterpret_cast<test_callback_fn*>(world_post_run));
  world->_on_error= reinterpret_cast<test_callback_error_fn*>(world_on_error);

  world->collection_startup= reinterpret_cast<test_callback_fn*>(world_container_startup);
  world->collection_shutdown= reinterpret_cast<test_callback_fn*>(world_container_shutdown);

  world->runner= &defualt_libmemcached_runner;
}
