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

#define LITERAL(s) (s),strlen(s)
#define LOOPED_SECTION(tests) \
  auto i_=0;                  \
  for (auto &&test : tests) DYNAMIC_SECTION("test" << i_++)

using namespace std;

using socket_or_port_t = variant<string, int>;

const char *getenv_else(const char *var, const char *defval);
unsigned random_num(unsigned min, unsigned max);
unsigned random_port();
string random_socket();
string random_socket_or_port_string(const string &what);
string random_socket_or_port_flag(const string &binary);

inline auto random_socket_or_port_arg() {
  return make_pair(&random_socket_or_port_flag, &random_socket_or_port_string);
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
