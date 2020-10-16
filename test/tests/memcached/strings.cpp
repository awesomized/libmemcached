#include "test/lib/common.hpp"

TEST_CASE("memcached_strings") {
  SECTION("behaviors") {
    string bad_string{libmemcached_string_behavior(MEMCACHED_BEHAVIOR_MAX)};
    for (auto b = 0; b < MEMCACHED_BEHAVIOR_MAX; ++b) {
      auto chk_string = libmemcached_string_behavior(static_cast<memcached_behavior_t>(b));
      INFO("missing entry for memcached_behavior_t(" << b << ")?");
      REQUIRE(chk_string);
      REQUIRE(bad_string != chk_string);
    }
  }
  SECTION("distributions") {
    string bad_string{libmemcached_string_distribution(MEMCACHED_DISTRIBUTION_CONSISTENT_MAX)};
    for (auto d = 0; d < MEMCACHED_DISTRIBUTION_CONSISTENT_MAX; ++d) {
      auto chk_string = libmemcached_string_distribution(static_cast<memcached_server_distribution_t>(d));
      INFO("missing entry for memcached_server_distribution_t(" << d << ")?");
      REQUIRE(chk_string);
      REQUIRE(bad_string != chk_string);
    }
  }
  SECTION("return codes") {
    MemcachedPtr memc;
    string bad_string{memcached_strerror(*memc, MEMCACHED_MAXIMUM_RETURN)};
    for (auto r = 0; r < MEMCACHED_MAXIMUM_RETURN; ++r) {
      auto chk_string = memcached_strerror(*memc, static_cast<memcached_return_t>(r));
      INFO("missing entry for memcached_return_t(" << r << ")?");
      REQUIRE(chk_string);
      REQUIRE(bad_string != chk_string);
    }
  }
}
