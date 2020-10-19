#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_noblock") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;
  auto timeout = GENERATE(0, -1);

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, timeout));

  auto num = timeout ? 10'000 : 1'000'000;
  auto hit = 0u;
  auto rnd = random_ascii_string(512);
  for (auto i = 0; i < num; ++i) {
    auto key = to_string(i);
    auto rc = memcached_set(memc, key.c_str(), key.length(), rnd.c_str(), rnd.length(), 1, 0);
    switch (rc) {
    case MEMCACHED_SUCCESS:
    case MEMCACHED_BUFFERED:
    case MEMCACHED_SERVER_TEMPORARILY_DISABLED:
      break;
    case MEMCACHED_TIMEOUT:
    case MEMCACHED_WRITE_FAILURE:
      if(!timeout) {
        --i;
      }
      ++hit;
      REQUIRE(true);
      break;
    default:
      REQUIRE(false);
    }
  }
  INFO("failures triggered: " << hit);
  if (timeout) {
    CHECK_FALSE(hit);
  } else {
    CHECK(hit > 0);
  }
}
