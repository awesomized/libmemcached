#include "test/lib/common.hpp"
#include "test/lib/ReturnMatcher.hpp"

TEST_CASE("memcached_regression_lp583031") {
  MemcachedPtr memc;
  LoneReturnMatcher test{*memc};

  REQUIRE_SUCCESS(memcached_server_add(*memc, "192.0.2.1", 11211));
  REQUIRE_SUCCESS(memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 3000));
  REQUIRE_SUCCESS(memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 1000));
  REQUIRE_SUCCESS(memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 1000));
  REQUIRE_SUCCESS(memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 1000));
  REQUIRE_SUCCESS(memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 1000));
  REQUIRE_SUCCESS(memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 3));

  memcached_return_t rc;
  Malloced val(memcached_get(*memc, S("not-found"), nullptr, nullptr, &rc));
  REQUIRE_RC(MEMCACHED_TIMEOUT, rc);
  REQUIRE_FALSE(*val);
}
