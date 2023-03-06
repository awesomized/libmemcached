#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_gh-php-memcached_0531") {
  auto test = MemcachedCluster{Cluster{Server{MEMCACHED_BINARY, {"-p", random_socket_or_port_string("-p")}}, 1}};
  auto memc = &test.memc;
  auto nonblock = GENERATE(0, 1);

  INFO("nonblock=" << nonblock);

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, nonblock));

  REQUIRE_SUCCESS(memcached_set(memc, S("first"), S("first"), 0, 0));
  REQUIRE_SUCCESS(memcached_set(memc, S("second"), S("second"), 0, 0));

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 0));

  memcached_return_t rc;
  size_t len;
  Malloced value1(memcached_get(memc, S("first"), &len, nullptr, &rc));
  REQUIRE_RC(MEMCACHED_TIMEOUT, rc);
  REQUIRE_FALSE(*value1);

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 100));

  Malloced value2(memcached_get(memc, S("second"), &len, nullptr, &rc));
  REQUIRE_SUCCESS(rc);
  REQUIRE(string(*value2) == string("second"));
}
