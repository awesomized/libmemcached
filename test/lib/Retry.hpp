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

#include "common.hpp"

class Retry {
public:
  using predicate = function<bool()>;

  explicit Retry(predicate pred_, unsigned max_ = 10, chrono::milliseconds sleep_for_ = 100ms);

  bool operator()();

private:
  const unsigned growth = 3;
  unsigned max;
  chrono::milliseconds sleep_for;
  predicate pred;
};
