#include "testing/lib/common.hpp"
#include "testing/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_lp434484") {
  MemcachedCluster test;
  auto memc = &test.memc;

  test.enableBinaryProto();

  REQUIRE_RC(MEMCACHED_NOTFOUND, memcached_append(memc, S(__func__), S(__func__), 0, 0));

  vector<char> blob;
  blob.resize(2048 * 1024);
  REQUIRE_RC(MEMCACHED_E2BIG, memcached_set(memc, S(__func__), blob.data(), blob.size(), 0, 0));

}
