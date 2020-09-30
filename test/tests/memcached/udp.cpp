#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_udp") {
  auto test = MemcachedCluster::udp();
  auto memc = &test.memc;

  SECTION("sets reply flag") {
    // FIXME: bad internals test
    REQUIRE(memc->flags.reply);
    REQUIRE_FALSE(memc->flags.use_udp);
    REQUIRE_FALSE(memc->flags.use_udp == memc->flags.reply);
    test.enableUdp();
    REQUIRE_FALSE(memc->flags.reply);
    REQUIRE(memc->flags.use_udp);
    REQUIRE_FALSE(memc->flags.use_udp == memc->flags.reply);
    test.enableUdp(false);
    REQUIRE(memc->flags.reply);
    REQUIRE_FALSE(memc->flags.use_udp);
    REQUIRE_FALSE(memc->flags.use_udp == memc->flags.reply);
  }

  test.enableUdp();

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
    auto binary = GENERATE(0,1);

    test.enableBinaryProto(binary);

    DYNAMIC_SECTION("binary=" << binary) {
      SECTION("set") {
        for (auto i = 0; i < max; ++i) {
          auto s = to_string(i);
          INFO("i=" << i);
          REQUIRE_SUCCESS(memcached_set(memc, s.c_str(), s.length(), s.c_str(), s.length(), 0, 0));
        }
        // FIXME: check request id
        memcached_quit(memc);
        REQUIRE_SUCCESS(memcached_last_error(memc));
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
    }
  }
}
