#include "ReturnMatcher.hpp"

ReturnMatcher &ReturnMatcher::operator=(ReturnMatcher &&rm) {
  memc = exchange(rm.memc, nullptr);
  expected = rm.expected;
  return *this;
}

bool ReturnMatcher::match(const memcached_return_t &arg) const {
  return arg == expected;
}

ReturnMatcher ReturnMatcher::success() {
  return ReturnMatcher{memc};
}

ReturnMatcher ReturnMatcher::operator()(memcached_return_t expected_) {
  return ReturnMatcher{memc, expected_};
}

string ReturnMatcher::describe() const {
  return "is " + to_string(expected)
         + "\n  actual: " + memcached_last_error_message(memc);
}

ReturnMatcher::ReturnMatcher(ReturnMatcher &&rm) {
  *this = move(rm);
}
