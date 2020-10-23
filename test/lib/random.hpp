/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include <cstddef>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <utility>

using namespace std;

using kv_pair = pair<string, string>;

template<typename T>
enable_if_t<is_integral_v<T>, T> random_num(T min, T max) {
  using namespace chrono;
  using rnd = mt19937_64;
  using dst = uniform_int_distribution<uintmax_t>;

  static auto time = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
  static auto seed = static_cast<rnd::result_type>(time.count());
  static auto rgen = rnd{seed};

  return dst(min, max)(rgen);
}

unsigned random_port();
string random_port_string(const string &);

char random_binary();
string random_binary_string(size_t len);
char random_ascii(char min = '!', char max = '~');
string random_ascii_string(size_t len, char min = '!', char max = '~');
kv_pair random_ascii_pair(size_t minlen = 1 << 2, size_t maxlen = 1 << 10);

template<template<typename> class Container>
auto random_ascii_pairs(size_t count, size_t minlen = 1 << 2, size_t maxlen = 1 << 10) {
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
