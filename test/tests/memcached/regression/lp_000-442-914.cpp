#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

#include <iomanip>

/* Original Comment:
 * The test case isn't obvious so I should probably document why
 * it works the way it does. Bug 442914 was caused by a bug
 * in the logic in memcached_purge (it did not handle the case
 * where the number of bytes sent was equal to the watermark).
 * In this test case, create messages so that we hit that case
 * and then disable noreply mode and issue a new command to
 * verify that it isn't stuck. If we change the format for the
 * delete command or the watermarks, we need to update this
 * test....
 */

TEST_CASE("memcached_regression_lp442914") {
  MemcachedCluster test{Cluster{Server{MEMCACHED_BINARY, {"-p", random_port_string}}, 1}};
  auto memc = &test.memc;

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NOREPLY, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, 1));


  for (auto x = 0; x < 250; ++x) {
    stringstream ss;
    ss << setfill('0') << setw(250) << x;
    REQUIRE_SUCCESS(memcached_delete(memc, ss.str().c_str(), ss.str().length(), 0));
  }

  stringstream key;
  key << setfill('0') << setw(37) << 251;
  REQUIRE_SUCCESS(memcached_delete(memc, key.str().c_str(), key.str().length(), 0));

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NOREPLY, 0));

  REQUIRE_RC(MEMCACHED_NOTFOUND, memcached_delete(memc, key.str().c_str(), key.str().length(), 0));
}
