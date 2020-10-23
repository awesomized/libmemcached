#include "ReturnMatcher.hpp"

bool ReturnMatcher::match(const memcached_return_t &arg) const {
  return arg == expected;
}

ReturnMatcher ReturnMatcher::success() {
  return ReturnMatcher{memc};
}

ReturnMatcher ReturnMatcher::operator()(memcached_return_t expected_) {
  return ReturnMatcher{memc, expected_};
}

ReturnMatcher &ReturnMatcher::operator=(memcached_st *memc_) {
  memc = memc_;
  return *this;
}

string ReturnMatcher::describe() const {
  return "is " + to_string(expected)
         + "\n  actual: " + memcached_last_error_message(memc);
}

