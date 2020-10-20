#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

TEST_CASE("memcached_servers") {
  SECTION("memcached_servers_parse") {
    SECTION("does not leak memory") {
      memcached_server_st *s = memcached_servers_parse("1.2.3.4:1234");
      REQUIRE(s);
      memcached_server_free(s);
    }
  }

  SECTION("memcached_server_list") {
    SECTION("append with weight - all zeros") {
      memcached_server_st *sl = memcached_server_list_append_with_weight(
          nullptr, nullptr, 0, 0, 0
      );
      REQUIRE(sl);
      memcached_server_list_free(sl);
    }
    SECTION("append with weight - host set only") {
      memcached_server_st *sl = memcached_server_list_append_with_weight(
          nullptr, "localhost", 0, 0, 0
      );
      REQUIRE(sl);
      memcached_server_list_free(sl);
    }
    SECTION("append with weight - error set only") {
      memcached_return_t rc;
      memcached_server_st *sl = memcached_server_list_append_with_weight(
          nullptr, nullptr, 0, 0, &rc
      );
      REQUIRE(sl);
      REQUIRE(MEMCACHED_SUCCESS == rc);
      memcached_server_list_free(sl);
    }
  }

  SECTION("no configured servers") {
    MemcachedPtr memc;

    REQUIRE(MEMCACHED_NO_SERVERS == memcached_increment(*memc, S("key"), 1, nullptr));
  }

  SECTION("sort") {
    MemcachedPtr memc;
    auto local_memc = *memc;
    LoneReturnMatcher test{local_memc};
    size_t bigger = 0; /* Prime the value for the test_true in server_display_function */
    memcached_server_fn callbacks[1]{server_sort_callback};

    REQUIRE_SUCCESS(memcached_behavior_set(local_memc, MEMCACHED_BEHAVIOR_SORT_HOSTS, 1));

    SECTION("variation 1") {
      for (uint32_t x = 0; x < 7; x++) {
        REQUIRE_SUCCESS(memcached_server_add_with_weight(local_memc, "localhost", random_port(), 0));
        REQUIRE(x + 1 == memcached_server_count(local_memc));
      }
      memcached_server_cursor(local_memc, callbacks, &bigger, 1);
    }

    SECTION("variation 2") {
      const memcached_instance_st *instance;

      REQUIRE_SUCCESS(memcached_server_add_with_weight(local_memc, "MEMCACHED_BEHAVIOR_SORT_HOSTS", 43043, 0));
      instance = memcached_server_instance_by_position(local_memc, 0);
      REQUIRE(in_port_t(43043) ==  memcached_server_port(instance));

      REQUIRE_SUCCESS(memcached_server_add_with_weight(local_memc, "MEMCACHED_BEHAVIOR_SORT_HOSTS", 43042, 0));

      instance = memcached_server_instance_by_position(local_memc, 0);
      REQUIRE(in_port_t(43042) ==  memcached_server_port(instance));

      instance = memcached_server_instance_by_position(local_memc, 1);
      REQUIRE(in_port_t(43043) ==  memcached_server_port(instance));

      memcached_server_cursor(local_memc, callbacks, &bigger, 1);
    }
  }

  SECTION("add null/empty") {
    MemcachedPtr memc;

    REQUIRE(MEMCACHED_SUCCESS == memcached_server_add(*memc, nullptr, 0));
    REQUIRE(1 == memcached_server_count(*memc));

    REQUIRE(MEMCACHED_SUCCESS == memcached_server_add(*memc, "", 0));
    REQUIRE(2 == memcached_server_count(*memc));
  }

  SECTION("add many") {
    MemcachedPtr memc;

    for (auto i = 0; i < 100; ++i) {
      REQUIRE(MEMCACHED_SUCCESS == memcached_server_add(*memc, "memc", 11211 + i));
    }
    REQUIRE(100 == memcached_server_count(*memc));
  }

  SECTION("add many weighted") {
    MemcachedPtr memc;

    for (auto i = 0; i < 100; ++i) {
      REQUIRE(MEMCACHED_SUCCESS == memcached_server_add_with_weight(*memc, "memc", 11211 +i, i % 10));
    }
    REQUIRE(100 == memcached_server_count(*memc));
  }

  SECTION("reset") {
    MemcachedPtr memc;

    REQUIRE_FALSE(memcached_server_count(*memc));
    REQUIRE(MEMCACHED_SUCCESS == memcached_server_add(*memc, "local", 12345));
    REQUIRE(1 == memcached_server_count(*memc));
    memcached_servers_reset(*memc);
    REQUIRE_FALSE(memcached_server_count(*memc));
    REQUIRE(MEMCACHED_SUCCESS == memcached_server_add(*memc, "local", 12345));
    REQUIRE(1 == memcached_server_count(*memc));
    memcached_servers_reset(*memc);
    REQUIRE_FALSE(memcached_server_count(*memc));
  }
}
