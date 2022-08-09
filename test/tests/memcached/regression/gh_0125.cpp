#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_gh_0125") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;
  auto blob = random_ascii_string(1024);
  auto binary = GENERATE(0, 1);

  test.enableBinaryProto(binary);
  INFO("binary: " << binary);

  memcached_return_t rc = memcached_set(memc, S("key"), blob.c_str(), blob.length(), -123, 0);
  REQUIRE_SUCCESS(rc);
}
