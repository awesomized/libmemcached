#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_udp") {
  auto test = MemcachedCluster::udp();
  auto memc = &test.memc;
  MemcachedPtr check;

  for (const auto &server : test.cluster.getServers()) {
    memcached_server_add(*check, "localhost", get<int>(server.getSocketOrPort()));
  }

  SECTION("compat") {
    memcached_return_t rc;
    REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS, memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, true));
    REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_version(memc));
    REQUIRE_SUCCESS(memcached_verbosity(memc, 0));
    REQUIRE(nullptr == memcached_get(memc, S(__func__), 0, 0, &rc));
    REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, rc);
    REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_mget_execute_by_key(memc, S(__func__), nullptr, nullptr, 0, nullptr, nullptr, 0));
    REQUIRE(nullptr == memcached_stat(memc, nullptr, &rc));
    REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, rc);
  }

  SECTION("io") {
    const auto max = 1025; // request id rolls over at 1024
    auto binary = GENERATE(0, 1);

    test.enableBinaryProto(binary);

    DYNAMIC_SECTION("binary=" << binary) {
      SECTION("set") {
        for (auto i = 0; i < max; ++i) {
          auto s = to_string(i);
          INFO("i=" << i);
          REQUIRE_SUCCESS(memcached_set(memc, s.c_str(), s.length(), s.c_str(), s.length(), 0, 0));
        }
        memcached_quit(memc);
        REQUIRE_SUCCESS(memcached_last_error(memc));

        for (auto i = 0; i < max; ++i) {
          auto s = to_string(i);
          memcached_return_t rc;
          INFO("i=" << i);
          Malloced val(memcached_get(*check, s.c_str(), s.length(), nullptr, nullptr, &rc));
          CHECK(*val);
          CHECK(MEMCACHED_SUCCESS == rc);
        }
      }

      SECTION("set too big") {
        const auto len = 1'234'567;
        auto blob = make_unique<char>(len);
        REQUIRE_RC(MEMCACHED_WRITE_FAILURE, memcached_set(memc, S(__func__), blob.get(), len, 0, 0));
        memcached_quit(memc);
        REQUIRE_SUCCESS(memcached_last_error(memc));
      }

      SECTION("delete") {
        for (auto i = 0; i < max; ++i) {
          auto s = to_string(i);
          INFO("i=" << i);
          REQUIRE_SUCCESS(memcached_delete(memc, s.c_str(), s.length(), 0));
        }
        memcached_quit(memc);
        REQUIRE_SUCCESS(memcached_last_error(memc));
      }

      SECTION("verbosity") {
        for (auto i = 0; i < max; ++i) {
          INFO("i=" << i);
          REQUIRE_SUCCESS(memcached_verbosity(memc, 0));
        }
        memcached_quit(memc);
        REQUIRE_SUCCESS(memcached_last_error(memc));
      }

      SECTION("flush") {
        for (auto i = 0; i < max; ++i) {
          INFO("i=" << i);
          REQUIRE_SUCCESS(memcached_flush(memc, 0));
        }
        memcached_quit(memc);
        REQUIRE_SUCCESS(memcached_last_error(memc));
      }

      SECTION("incremend/decrement") {
        uint64_t newval = 0;
        REQUIRE_SUCCESS(memcached_set(memc, S("udp-incr"), S("1"), 0, 0));
        memcached_quit(memc);
        this_thread::sleep_for(1s);
        REQUIRE_SUCCESS(memcached_increment(memc, S("udp-incr"), 1, &newval));
        memcached_quit(memc);
        this_thread::sleep_for(1s);
        REQUIRE(newval == UINT64_MAX);
        memcached_return_t rc;
        Malloced val(memcached_get(*check, S("udp-incr"), nullptr, nullptr, &rc));
        REQUIRE_SUCCESS(rc);
        REQUIRE(*val);
        CHECK(string(*val) == "2");
        REQUIRE_SUCCESS(memcached_decrement(memc, S("udp-incr"), 1, &newval));
        memcached_quit(memc);
        this_thread::sleep_for(1s);
        REQUIRE(newval == UINT64_MAX);
        val = memcached_get(*check, S("udp-incr"), nullptr, nullptr, &rc);
        REQUIRE_SUCCESS(rc);
        REQUIRE(*val);
        CHECK(string(*val) == "1");
      }
    }
  }
}
