#include "testing/lib/common.hpp"
#include "testing/lib/MemcachedCluster.hpp"

TEST_CASE("memcached regression binary_block_add") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;
  auto blob = random_ascii_string(1024);

  test.enableBinaryProto();

  for (auto i = 0; i < 20480; ++i) {
    auto rkey = random_ascii_string(12);
    memcached_return_t rc = memcached_add_by_key(memc, S("key"), rkey.c_str(), rkey.length(), blob.c_str(), blob.length(), 0, 0);

    if (rc == MEMCACHED_MEMORY_ALLOCATION_FAILURE) {
      break;
    } else if (rc != MEMCACHED_DATA_EXISTS) {
      REQUIRE_SUCCESS(rc);
    }
  }
}
