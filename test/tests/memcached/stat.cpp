#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

static memcached_return_t item_counter(const memcached_instance_st *, const char *key, size_t, const char *value, size_t, void *context)
{
  if (key == "curr_items"s) {
    auto counter = static_cast<size_t *>(context);
    *counter += stoul(value);
  }

  return MEMCACHED_SUCCESS;
}

static memcached_return_t stat_null(const memcached_instance_st *, const char *, size_t, const char *, size_t, void *) {
  return MEMCACHED_SUCCESS;
}

TEST_CASE("memcached_stat") {
  MemcachedCluster test;
  auto memc = &test.memc;

  SECTION("invalid arguments") {
    size_t count = 0;
    REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS, memcached_stat_execute(memc, "BAD_ARG", item_counter, &count));
  }

  SECTION("execute") {
    for (auto i = 0; i < 64; ++i) {
      auto key = random_ascii_string(12) + to_string(i);
      REQUIRE_SUCCESS(memcached_set(memc, key.c_str(), key.length(), nullptr, 0, 0, 0));
    }

    memcached_quit(memc);

    size_t count = 0;
    REQUIRE_SUCCESS(memcached_stat_execute(memc, nullptr, item_counter, &count));
    REQUIRE(count == 64);

    auto arg = GENERATE(as<string>(), "slabs", "items", "sizes");
    REQUIRE_SUCCESS(memcached_stat_execute(memc, arg.c_str(), stat_null, nullptr));
  }

  SECTION("servername") {
    for (auto &server : test.cluster.getServers()) {
      auto port = server.getSocketOrPort();
      if (holds_alternative<int>(port)) {
        memcached_stat_st stats;
        REQUIRE_SUCCESS(memcached_stat_servername(&stats, nullptr, "localhost", get<int>(port)));
        REQUIRE(stats.pid);
      }
    }
  }

  SECTION("get_keys/get_value") { // oh my
    memcached_return_t rc;
    auto servers = test.cluster.getServers();
    Malloced stats(memcached_stat(memc, nullptr, &rc));
    Malloced keys(memcached_stat_get_keys(memc, nullptr, nullptr));

    REQUIRE_SUCCESS(rc);
    REQUIRE(*stats);
    REQUIRE(*keys);

    for (auto i = 0U; i < memcached_server_count(memc); ++i) {
      auto this_stat = &(*stats)[i];

      for (auto key = *keys; *key; ++key) {
        Malloced val(memcached_stat_get_value(memc, this_stat, *key, &rc));
        REQUIRE_SUCCESS(rc);
        REQUIRE(*val);
      }
      for (auto &server : test.cluster.getServers()) {
        if (this_stat->pid == server.getPid()) {
          goto found;
        }
      }

      FAIL("no server matches pid " << this_stat->pid);
      found:;
    }
  }
}
