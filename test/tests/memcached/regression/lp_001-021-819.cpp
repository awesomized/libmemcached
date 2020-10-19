#include "test/lib/common.hpp"

TEST_CASE("memcached_regression_lp1021819") {
  MemcachedPtr memc;

  REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 2'000'000));
  REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 2'000'000));

  memcached_return_t rc;
  Malloced val(memcached_get(*memc, S("not-found"), nullptr, nullptr, &rc));
  REQUIRE(MEMCACHED_NOTFOUND == rc);
  REQUIRE_FALSE(*val);
}
