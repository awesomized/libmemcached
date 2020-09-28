#pragma once

#include "test/lib/common.hpp"

class ReturnMatcher : public Catch::MatcherBase<memcached_return_t> {
public:
  explicit ReturnMatcher(const memcached_st *memc_, memcached_return_t expected_ = MEMCACHED_SUCCESS)
      : memc{memc_}
      , expected{expected_}
  {}

  ReturnMatcher(const ReturnMatcher &) = default;
  ReturnMatcher &operator = (const ReturnMatcher &) = default;

  ReturnMatcher(ReturnMatcher &&rm);
  ReturnMatcher &operator = (ReturnMatcher &&rm);

  bool match(const memcached_return_t &arg) const override;
  ReturnMatcher success();
  ReturnMatcher operator () (memcached_return_t expected_);

protected:
  string describe() const override;

private:
  const memcached_st *memc;
  memcached_return_t expected{MEMCACHED_SUCCESS};
};

class LoneReturnMatcher {
public:
  ReturnMatcher returns;
  explicit LoneReturnMatcher(const memcached_st *memc) : returns{memc}
  {}
};
