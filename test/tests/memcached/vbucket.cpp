#include "test/lib/common.hpp"
#include "test/lib/ReturnMatcher.hpp"


TEST_CASE("memcached_vbucket") {
  MemcachedPtr memc;
  LoneReturnMatcher test{*memc};
  uint32_t server_map[] = {0, 1, 2, 1};

  REQUIRE_SUCCESS(memcached_bucket_set(*memc, server_map, nullptr, 4, 2));
  REQUIRE(MEMCACHED_DISTRIBUTION_VIRTUAL_BUCKET == memcached_behavior_get_distribution(*memc));
  REQUIRE_SUCCESS(memcached_behavior_set_key_hash(*memc, MEMCACHED_HASH_CRC));

  auto servers = memcached_servers_parse("localhost:11211, localhost1:11210, localhost2:11211");
  REQUIRE(servers);
  REQUIRE_SUCCESS(memcached_server_push(*memc, servers));
  REQUIRE(3 == memcached_server_count(*memc));

  tuple<const char *, uint32_t, uint32_t> keys[] = {
      {"hello", 0, 0},
      {"doctor", 0, 0},
      {"name", 1, 3},
      {"continue", 1, 3},
      {"yesterday", 0, 0},
      {"tomorrow", 1, 1},
      {"another key", 2, 2}
  };

  for (const auto &k : keys) {
    auto [key, server, bucket] = k;

    REQUIRE(bucket == (memcached_generate_hash_value(S(key), memcached_behavior_get_key_hash(*memc)) % 4));
    REQUIRE(server == memcached_generate_hash(*memc, S(key)));
  }

  memcached_server_list_free(servers);
}
