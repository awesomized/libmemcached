#include "test/lib/common.hpp"

TEST_CASE("memcached_errors") {
  SECTION("NO_SERVERS") {
    MemcachedPtr memc;
    memcached_return_t rc;
    auto key = "key";
    size_t len = 3;

    REQUIRE(MEMCACHED_NO_SERVERS == memcached_flush(*memc, 0));
    REQUIRE(MEMCACHED_NO_SERVERS == memcached_set(*memc, S(__func__), S(__func__), 0, 0));
    REQUIRE_FALSE(memcached_get(*memc, S(__func__), nullptr, nullptr, &rc));
    REQUIRE(MEMCACHED_NO_SERVERS == rc);
    REQUIRE(MEMCACHED_NO_SERVERS == memcached_mget(*memc, &key, &len, 1));
  }
}
