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

#include "testing/lib/catch.hpp"
#include "testing/lib/random.hpp"

#include "libmemcached/memcached.h"

using namespace std;
using socket_or_port_t = variant<string, int>;

/**
 * Useful macros for testing
 */
#define S(s) (s),strlen(s)
#define DECLARE_STREQUAL static auto strequal = equal_to<string>();
#define LOOPED_SECTION(tests) \
  for (auto &[name, test] : tests) DYNAMIC_SECTION("test " << name)
#define REQUIRE_SUCCESS(rc) do { \
    INFO("expected: SUCCESS");   \
    REQUIRE_THAT(rc, test.returns.success()); \
  } while(0)
#define REQUIRE_RC(rc, call) do { \
    INFO("expected: " << memcached_strerror(nullptr, rc)); \
    REQUIRE_THAT(call, test.returns(rc));                  \
  } while(0)

const char *getenv_else(const char *var, const char *defval);

inline memcached_return_t fetch_all_results(memcached_st *memc, unsigned int &keys_returned, memcached_return_t &rc) {
  keys_returned = 0;

  memcached_result_st *result = nullptr;
  while ((result = memcached_fetch_result(memc, result, &rc))) {
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

template<class T>
class Malloced {
  T *ptr;
public:
  Malloced(T *ptr_)
  : ptr{ptr_}
  {}
  ~Malloced() {
    if(ptr)
      free(ptr);
  }
  auto operator *() {
    return ptr;
  }
};
