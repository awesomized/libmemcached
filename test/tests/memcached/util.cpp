#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

#include "libmemcached/instance.hpp"
#include "libmemcachedutil/common.h"

TEST_CASE("memcached_util") {
  SECTION("version_check") {
    auto test = MemcachedCluster::mixed();
    auto memc = &test.memc;

    REQUIRE_SUCCESS(memcached_version(memc));
    REQUIRE(libmemcached_util_version_check(memc, 0, 0, 0));
    REQUIRE_FALSE(libmemcached_util_version_check(memc, 255, 255, 255));

    auto instance = memcached_server_instance_by_position(memc, 0);
    REQUIRE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version, instance->micro_version));
    if (instance->micro_version) {
      REQUIRE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version, instance->micro_version - 1));
      if (instance->micro_version < 255) {
        REQUIRE_FALSE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version, instance->micro_version + 1));
      }
    }
    if (instance->minor_version) {
      REQUIRE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version - 1, 255));
      if (instance->minor_version < 255) {
        REQUIRE_FALSE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version + 1, 0));
      }
    }
    if (instance->major_version) {
      REQUIRE(libmemcached_util_version_check(memc, instance->major_version - 1, 255, 255));
      if (instance->major_version < 255) {
        REQUIRE_FALSE(libmemcached_util_version_check(memc, instance->major_version + 1, 0, 0));
      }
    }
  }

  // see sasl.cpp for getpid2 test
  SECTION("getpid") {
    auto test = MemcachedCluster::network();
    memcached_return_t rc;

    for (auto &server : test.cluster.getServers()) {
      REQUIRE(server.getPid() == libmemcached_util_getpid("localhost", get<int>(server.getSocketOrPort()), &rc));
      REQUIRE_SUCCESS(rc);
    }

    REQUIRE(-1 == libmemcached_util_getpid("localhost", 1, &rc));
    REQUIRE(memcached_fatal(rc));
  }

  // see sasl.cpp for ping2 test
  SECTION("ping") {
    auto test = MemcachedCluster::network();
    auto memc = &test.memc;
    memcached_server_fn fptr[] = {&ping_callback};

    REQUIRE_SUCCESS(memcached_server_cursor(memc, fptr, nullptr, 1));
  }

  SECTION("flush") {
    auto test = MemcachedCluster::network();

    for (auto &server : test.cluster.getServers()) {
      memcached_return_t rc;
      REQUIRE(libmemcached_util_flush("localhost", get<int>(server.getSocketOrPort()), &rc));
      REQUIRE_SUCCESS(rc);
    }
  }

  SECTION("pool") {
    SECTION("deprecated") {
      auto conf = "--SERVER=host10.example.com --SERVER=host11.example.com --SERVER=host10.example.com --POOL-MIN=10 --POOL-MAX=32";
      auto pool = memcached_pool(S(conf));
      REQUIRE(pool);

      memcached_return_t rc;
      auto memc = memcached_pool_pop(pool, false, &rc);
      REQUIRE(memc);
      REQUIRE(MEMCACHED_SUCCESS == rc);

      REQUIRE(MEMCACHED_SUCCESS == memcached_pool_push(pool, memc));
      REQUIRE(nullptr == memcached_pool_destroy(pool));
    }

    SECTION("basic") {
      auto test = MemcachedCluster::mixed();
      auto memc = &test.memc;
      memcached_return_t rc;

      constexpr auto POOL_MIN = 5;
      constexpr auto POOL_MAX = 10;

      auto pool = memcached_pool_create(memc, POOL_MIN, POOL_MAX);
      REQUIRE(pool);

      array<memcached_st *, POOL_MAX> hold{};
      for (auto &h : hold) {
        h = memcached_pool_fetch(pool, nullptr, &rc);
        REQUIRE_SUCCESS(rc);
        REQUIRE(h);
      }

      SECTION("depleted") {
        REQUIRE(nullptr == memcached_pool_fetch(pool, nullptr, &rc));
        REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
      }

      SECTION("usable") {
        for (auto h : hold) {
          auto s = to_string(reinterpret_cast<uintptr_t>(h));
          REQUIRE_SUCCESS(memcached_set(h, s.c_str(), s.length(), s.c_str(), s.length(), 0, 0));
        }
        for (auto h : hold) {
          auto s = to_string(reinterpret_cast<uintptr_t>(h));
          Malloced val(memcached_get(h, s.c_str(), s.length(), nullptr, nullptr, &rc));
          REQUIRE_SUCCESS(rc);
          REQUIRE(*val);
          REQUIRE(h == reinterpret_cast<memcached_st *>(stoul(*val)));
        }

        REQUIRE_SUCCESS(memcached_set(hold[0], S(__func__), "0", 1, 0, 0));
        uint64_t inc = 0;
        for (auto h : hold) {
          uint64_t val;
          REQUIRE_SUCCESS(memcached_increment(h, S(__func__), 1, &val));
          CHECK(++inc == val);
        }
      }

      SECTION("behaviors") {
        uint64_t val;
        REQUIRE_SUCCESS(memcached_pool_behavior_get(pool, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK, &val));
        REQUIRE_FALSE(val == 9999);
        REQUIRE_SUCCESS(memcached_pool_behavior_set(pool, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK, 9999));
        REQUIRE_SUCCESS(memcached_pool_behavior_get(pool, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK, &val));
        REQUIRE(val == 9999);

        for (auto &h : hold) {
          REQUIRE_FALSE(9999 == memcached_behavior_get(h, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK));
          REQUIRE_SUCCESS(memcached_pool_release(pool, h));
          h = memcached_pool_fetch(pool, nullptr, &rc);
          REQUIRE_SUCCESS(rc);
          REQUIRE(h);
          REQUIRE(9999 == memcached_behavior_get(h, MEMCACHED_BEHAVIOR_IO_MSG_WATERMARK));
          REQUIRE_FALSE(9999 == memcached_behavior_get(h, MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK));
          REQUIRE_SUCCESS(memcached_pool_release(pool, h));
        }

        REQUIRE_SUCCESS(memcached_pool_behavior_get(pool, MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK, &val));
        REQUIRE_FALSE(val == 9999);
        REQUIRE_SUCCESS(memcached_pool_behavior_set(pool, MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK, 9999));
        REQUIRE_SUCCESS(memcached_pool_behavior_get(pool, MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK, &val));
        REQUIRE(val == 9999);

        for (auto &h : hold) {
          h = memcached_pool_fetch(pool, nullptr, &rc);
          REQUIRE_SUCCESS(rc);
          REQUIRE(h);
          REQUIRE(9999 == memcached_behavior_get(h, MEMCACHED_BEHAVIOR_IO_BYTES_WATERMARK));
        }
      }

      for (auto h : hold) {
        REQUIRE_SUCCESS(memcached_pool_release(pool, h));
        auto again = memcached_pool_fetch(pool, nullptr, &rc);
        REQUIRE_SUCCESS(rc);
        REQUIRE(again);
        REQUIRE_SUCCESS(memcached_pool_release(pool, again));
      }

      REQUIRE(memc == memcached_pool_destroy(pool));
    }
  }
}
