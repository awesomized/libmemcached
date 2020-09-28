#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_lp996813") {
  MemcachedPtr memc_ptr;
  auto memc = *memc_ptr;
  LoneReturnMatcher test{memc};

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 300));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 30));

  // We will never connect to these servers
  in_port_t base_port = 11211;
  for (size_t x = 0; x < 17; x++) {
    REQUIRE_SUCCESS(memcached_server_add(memc, "10.2.3.4", base_port + x));
  }

  REQUIRE(6U == memcached_generate_hash(memc, S("SZ6hu0SHweFmpwpc0w2R")));
  REQUIRE(1U == memcached_generate_hash(memc, S("SQCK9eiCf53YxHWnYA.o")));
  REQUIRE(9U == memcached_generate_hash(memc, S("SUSDkGXuuZC9t9VhMwa.")));
  REQUIRE(0U == memcached_generate_hash(memc, S("SnnqnJARfaCNT679iAF_")));
}
