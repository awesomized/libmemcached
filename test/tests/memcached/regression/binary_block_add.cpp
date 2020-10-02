#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_binary_block_add") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;
  auto blob = random_ascii_string(1024);
  auto binary = GENERATE(0, 1);

  test.enableBinaryProto(binary);
  INFO("binary: " << binary);

  for (auto i = 0; i < 20480; ++i) {
    auto rkey = random_ascii_string(12) + to_string(i);
    memcached_return_t rc = memcached_add_by_key(memc, S("key"), rkey.c_str(), rkey.length(), blob.c_str(), blob.length(), 0, 0);

    if (rc == MEMCACHED_MEMORY_ALLOCATION_FAILURE) {
      break;
    } else if (rc != MEMCACHED_DATA_EXISTS) {
      REQUIRE_SUCCESS(rc);
    }
  }
}
