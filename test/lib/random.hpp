#pragma once

#include <cstddef>
#include <chrono>
#include <random>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

using kv_pair = pair<string, string>;

template<typename T>
enable_if_t<is_integral_v<T>, T> random_num(T min, T max) {
  using namespace chrono;
  using rnd = mt19937;
  using dst = uniform_int_distribution<T>;

  auto time = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
  auto seed = static_cast<rnd::result_type>(time.count() % numeric_limits<T>::max());
  auto rgen = rnd{seed};
  return dst(min, max)(rgen);
}

unsigned random_port();
string random_port_string(const string &);

char random_ascii(char min = '!', char max = '~');
string random_ascii_string(size_t len, char min = '!', char max = '~');
kv_pair random_ascii_pair(size_t minlen = 1<<2, size_t maxlen = 1<<10);

template<template <typename> class Container>
auto random_ascii_pairs(size_t count, size_t minlen = 1<<2, size_t maxlen = 1<<10) {
  Container<kv_pair> v;

  v.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    v.emplace_back(random_ascii_pair(minlen, maxlen));
  }

  return v;
}

string random_socket(const string &prefix = "/tmp/libmc.");
string random_socket_or_port_string(const string &what);
string random_socket_or_port_flag(const string &binary);

inline auto random_socket_or_port_arg() {
  return make_pair(&random_socket_or_port_flag, &random_socket_or_port_string);
}
