#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

#include "libmemcached/instance.hpp"

TEST_CASE("memcached_replication") {
  MemcachedCluster test = MemcachedCluster::network();
  auto memc = &test.memc;

  test.enableBinaryProto();
  test.enableReplication();

  SECTION("replicates one key to all servers") {
    REQUIRE_SUCCESS(memcached_set(memc, S(__func__), S(__func__), 0, 0));
    memcached_quit(memc);

    for (const auto &server : test.cluster.getServers()) {
      MemcachedPtr check;

      REQUIRE_SUCCESS(memcached_server_add(*check, "localhost", get<int>(server.getSocketOrPort())));

      memcached_return_t rc;
      Malloced val(memcached_get(*check, S(__func__), nullptr, nullptr, &rc));
      REQUIRE_SUCCESS(rc);
      REQUIRE(string(__func__) == *val);
    }
  }

#define NUM_KEYS 100

  SECTION("replicates many keys to all servers") {
    array<string, NUM_KEYS> str;
    array<char *, NUM_KEYS> chr;
    array<size_t, NUM_KEYS> len;

    for (auto i = 0U; i < NUM_KEYS; ++i) {
      str[i] = random_binary_string(12) + to_string(i);
      chr[i] = str[i].data();
      len[i] = str[i].length();

      REQUIRE_SUCCESS(memcached_set(memc, chr[i], len[i], chr[i], len[i], 0, 0));
    }
    memcached_quit(memc);

    for (auto i = 0U; i < memcached_server_count(memc); ++i) {
      auto pos = i % memcached_server_count(memc);
      auto ins = memcached_server_instance_by_position(memc, pos);
      MemcachedPtr single;
      memcached_result_st r;
      memcached_return_t rc;

      REQUIRE(memcached_result_create(*single, &r));
      REQUIRE_SUCCESS(memcached_server_add(*single, "localhost", ins->port()));
      REQUIRE_SUCCESS(memcached_behavior_set(*single, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1));
      REQUIRE_SUCCESS(memcached_mget(*single, chr.data(), len.data(), NUM_KEYS));

      size_t n = 0;
      while (memcached_fetch_result(*single, &r, &rc)) {
        ++n;
        REQUIRE_SUCCESS(rc);
      }
      CHECK(n == NUM_KEYS);
      REQUIRE_RC(MEMCACHED_END, rc);
      memcached_result_free(&r);
    }

    SECTION("deletes and randomize reads") {
      REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RANDOMIZE_REPLICA_READ, 1));
      for (auto i = 0; i < NUM_KEYS; ++i) {
        memcached_return_t rc;
        auto result = memcached_result_create(memc, nullptr);

        REQUIRE(result);
        REQUIRE_SUCCESS(memcached_mget_by_key(memc, chr[i], len[i], chr.data(), len.data(), NUM_KEYS));

        size_t n = 0;
        while (memcached_fetch_result(memc, result, &rc)) {
          ++n;
          REQUIRE_SUCCESS(rc);
        }
        CHECK(n + i == NUM_KEYS);
        REQUIRE_RC(MEMCACHED_END, rc);

        REQUIRE_SUCCESS(memcached_delete(memc, chr[i], len[i], 0));
      }
    }

#if 0
# warning I think the old test is bogus and the functionality does not exist as advertised

    SECTION("deleted from primary") {
      for (auto i = 0; i < NUM_KEYS; ++i) {
        memcached_return_t rc;

        CHECK(MEMCACHED_SUCCESS == memcached_delete(memc, chr[i], len[i], 0));
        Malloced val(memcached_get(memc, chr[i], len[i], nullptr, nullptr, &rc));
        CHECK(MEMCACHED_SUCCESS == rc);
        CHECK(*val);
      }
    }
    SECTION("dead replica") {
      test.killOneServer();

      for (auto i = 0; i < NUM_KEYS; ++i) {
        memcached_return_t rc;

        Malloced val(memcached_get(memc, chr[i], len[i], nullptr, nullptr, &rc));
        CHECK(MEMCACHED_SUCCESS == rc);
        CHECK(*val);
      }
    }
#endif
  }
}
