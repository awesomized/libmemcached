#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <variant>
#include <vector>

#include "../lib/catch.hpp"

#include "libmemcached/memcached.h"

using namespace std;
using socket_or_port_t = variant<string, int>;

/**
 * Useful macros for testing
 */
#define S(s) (s),strlen(s)
#define LOOPED_SECTION(tests) \
  for (auto &[name, test] : tests) DYNAMIC_SECTION("test " << name)
#define REQUIRE_SUCCESS(rc) REQUIRE_THAT(rc, test.returns.success())
#define REQUIRE_RC(rc, call) REQUIRE_THAT(call, test.returns(rc))


const char *getenv_else(const char *var, const char *defval);
unsigned random_num(unsigned min, unsigned max);
unsigned random_port();
string random_socket();
string random_socket_or_port_string(const string &what);
string random_socket_or_port_flag(const string &binary);

inline auto random_socket_or_port_arg() {
  return make_pair(&random_socket_or_port_flag, &random_socket_or_port_string);
}

inline memcached_return_t fetch_all_results(memcached_st *memc, unsigned int &keys_returned, memcached_return_t &rc) {
  keys_returned = 0;

  memcached_result_st *result = nullptr;
  while ((result = memcached_fetch_result(memc, result, &rc)))
  {
    REQUIRE(MEMCACHED_SUCCESS == rc);
    keys_returned += 1;
  }
  memcached_result_free(result);
  return MEMCACHED_SUCCESS;
}

inline memcached_return_t fetch_all_results(memcached_st *memc, unsigned int &keys_returned) {
  memcached_return_t rc;
  fetch_all_results(memc, keys_returned, rc);
  return rc;
}

class MemcachedPtr {
public:
  memcached_st memc;

  explicit
  MemcachedPtr(memcached_st *memc_) {
    memset(&memc, 0, sizeof(memc));
    REQUIRE(memcached_clone(&memc, memc_));
  }
  MemcachedPtr()
  : MemcachedPtr(nullptr)
  {}
  ~MemcachedPtr() {
    memcached_free(&memc);
  }
  memcached_st *operator * () {
    return &memc;
  }
};

class Malloced {
  void *ptr;
public:
  Malloced(void *ptr_)
  : ptr{ptr_}
  {}
  ~Malloced() {
    free(ptr);
  }
  void *operator *() {
    return ptr;
  }
};
