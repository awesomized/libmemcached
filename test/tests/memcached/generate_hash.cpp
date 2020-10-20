#include "test/lib/common.hpp"
#include "test/fixtures/hashes.hpp"

static constexpr const uint32_t md5_hosts[] = {4U, 1U, 0U, 1U, 4U, 2U, 0U, 3U, 0U, 0U, 3U, 1U, 0U, 0U, 1U, 3U, 0U, 0U, 0U, 3U, 1U, 0U, 4U, 4U, 3U, 1U};
static constexpr const uint32_t crc_hosts[] = {2U, 4U, 1U, 0U, 2U, 4U, 4U, 4U, 1U, 2U, 3U, 4U, 3U, 4U, 1U, 3U, 3U, 2U, 0U, 0U, 0U, 1U, 2U, 4U, 0U, 4U};
static constexpr const uint32_t *hosts[] = {nullptr, md5_hosts, crc_hosts};

TEST_CASE("memcached_generate_hash") {
  MemcachedPtr memc(memcached(S("--server=localhost:1 --server=localhost:2 --server=localhost:3 --server=localhost:4 --server=localhost5 --DISTRIBUTION=modula")));

  REQUIRE(*memc);

  SECTION("generate hash value") {
    for (int f = MEMCACHED_HASH_DEFAULT; f < MEMCACHED_HASH_MAX; ++f) {
      auto h = static_cast<memcached_hash_t>(f);

      if (h == MEMCACHED_HASH_CUSTOM) {
        continue;
      }
      if (MEMCACHED_SUCCESS != memcached_behavior_set_key_hash(*memc, h)) {
        WARN("hash algorithm not enabled: " << libmemcached_string_hash(h) << " (" << f << ")");
        continue;
      }

      INFO("hash: " << libmemcached_string_hash(h));

      auto n = 0;
      for (auto i : input) {
        CHECK(output[f][n] == memcached_generate_hash_value(S(i), h));
        ++n;
      }
    }
  }

  SECTION("generate hash") {
    auto hash = GENERATE(as<memcached_hash_t>{}, MEMCACHED_HASH_MD5, MEMCACHED_HASH_CRC);

    INFO("hash: " << libmemcached_string_hash(hash));
    REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set_key_hash(*memc, hash));

    auto n = 0;
    for (auto i : input) {
      CHECK(hosts[hash][n] == memcached_generate_hash(*memc, S(i)));
      ++n;
    }
  }
}
