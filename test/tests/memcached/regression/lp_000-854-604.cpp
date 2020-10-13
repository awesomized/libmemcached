#include "test/lib/common.hpp"

#define NULL_CSTR nullptr,0

TEST_CASE("memcached_regression_lp854604") {
  char buffer[1024];

  REQUIRE(MEMCACHED_INVALID_ARGUMENTS == libmemcached_check_configuration(NULL_CSTR, NULL_CSTR));
  REQUIRE(MEMCACHED_INVALID_ARGUMENTS == libmemcached_check_configuration(NULL_CSTR, buffer, 0));
  for (auto i = 0; i < 100; ++i) {
    REQUIRE(MEMCACHED_PARSE_ERROR == libmemcached_check_configuration(S("syntax error"), buffer, random_num(0,1024)));
  }
}
