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

#include "test/lib/common.hpp"

class ReturnMatcher : public Catch::MatcherBase<memcached_return_t> {
public:
  explicit ReturnMatcher(memcached_st *memc_,
                         memcached_return_t expected_ = MEMCACHED_SUCCESS)
  : memc{memc_}
  , expected{expected_} {}

  ReturnMatcher(const ReturnMatcher &) = default;

  bool match(const memcached_return_t &arg) const override;
  ReturnMatcher success();
  ReturnMatcher operator()(memcached_return_t expected_);
  ReturnMatcher &operator=(memcached_st *memc_);

protected:
  string describe() const override;

private:
  memcached_st *memc;
  memcached_return_t expected{MEMCACHED_SUCCESS};
};

class LoneReturnMatcher {
public:
  ReturnMatcher returns;
  explicit LoneReturnMatcher(memcached_st *memc)
  : returns{memc} {}
};
