/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#pragma once

#include "time.hpp"
#include <random>
#undef max

class random64 {
public:
  using typ = std::mt19937_64::result_type;

  random64()
  : gen{static_cast<typ>(time_clock::now().time_since_epoch().count())}
  , dst{}
  {}

  typ operator()(typ min_ = 0, typ max_ = std::numeric_limits<typ>::max()) {
    return (dst(gen) % (max_ - min_)) + min_;
  }

  void fill(char *buf, size_t len,
      const std::string &set = "0123456789ABCDEFGHIJKLMNOPQRSTWXYZabcdefghijklmnopqrstuvwxyz") {
    for (auto i = 0ul; i < len; ++i) {
      buf[i] = set[(*this)(0, set.length()-1)];
    }
  }

private:
  std::mt19937_64 gen;
  std::uniform_int_distribution<typ> dst;
};
