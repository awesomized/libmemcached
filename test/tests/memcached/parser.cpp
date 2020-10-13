#include "test/lib/common.hpp"
#include "test/lib/env.hpp"

#include "libmemcached/common.h"
#include "test/fixtures/parser.hpp"

TEST_CASE("memcached_parser") {
  SECTION("fail: null string") {
    REQUIRE_FALSE(memcached(nullptr, 123));
  }
  SECTION("fail: zero length") {
    REQUIRE_FALSE(memcached("123", 0));
  }
  SECTION("success: localhost") {
    auto mc = memcached(S("--server=localhost"));
    REQUIRE(mc);
    memcached_free(mc);
  }
  SECTION("env") {
    SECTION("success: localhost") {
      SET_ENV(success_localhost, "LIBMEMCACHED", "--server=localhost");
      auto mc = memcached(nullptr, 0);
      REQUIRE(mc);
      memcached_free(mc);
    }
    SECTION("success: empty string") {
      SET_ENV(success_empty_string, "LIBMEMCACHED", "");
      auto mc = memcached(nullptr, 0);
      REQUIRE(mc);
      memcached_free(mc);
    }
    SECTION("fail: extra quotes") {
      SET_ENV(fail_extra_quotes, "LIBMEMCACHED", "\"--server=localhost\"");
      REQUIRE_FALSE(memcached(nullptr, 0));
    }
  }

  SECTION("fixtures") {
    for (const auto &test : tests) {
      DYNAMIC_SECTION(test.name) {
        for (size_t i = 0; i < test.ntests; ++i) {
          auto &tc = test.tests[i];
          DYNAMIC_SECTION(tc.option.c_str) {
            MemcachedPtr memc(memcached(tc.option.c_str, tc.option.size));
            test.check(*memc, tc.result);
          }
        }
      }
    }
  }
}
