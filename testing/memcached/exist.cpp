#include "../lib/common.hpp"
#include "../lib/MemcachedCluster.hpp"

TEST_CASE("memcached exist") {
  MemcachedCluster tests[]{
      MemcachedCluster::mixed(),
      MemcachedCluster::net(),
      MemcachedCluster::socket()
  };

  tests[0].enableBinary();

  LOOPED_SECTION(tests) {
    auto memc = &test.memc;

    SECTION("initial not found") {
      REQUIRE(
          MEMCACHED_NOTFOUND == memcached_exist(memc, LITERAL("frog")));
    }

    SECTION("set found") {
      REQUIRE(MEMCACHED_SUCCESS ==
              memcached_set(memc, LITERAL("frog"), LITERAL("frog"), 0,
                            0));
      REQUIRE(
          MEMCACHED_SUCCESS == memcached_exist(memc, LITERAL("frog")));

      SECTION("deleted not found") {
        REQUIRE(MEMCACHED_SUCCESS ==
                memcached_delete(memc, LITERAL("frog"), 0));
        REQUIRE(MEMCACHED_NOTFOUND ==
                memcached_exist(memc, LITERAL("frog")));
      }
    }

    SECTION("by key") {
      SECTION("initial not found") {
        REQUIRE(MEMCACHED_NOTFOUND ==
                memcached_exist_by_key(memc, LITERAL("master"),
                                       LITERAL("frog")));
      }

      SECTION("set found") {
        REQUIRE(MEMCACHED_SUCCESS ==
                memcached_set_by_key(memc, LITERAL("master"),
                                     LITERAL("frog"), LITERAL("frog"), 0, 0));
        REQUIRE(MEMCACHED_SUCCESS ==
                memcached_exist_by_key(memc, LITERAL("master"),
                                       LITERAL("frog")));

        SECTION("deleted not found") {
          REQUIRE(MEMCACHED_SUCCESS ==
                  memcached_delete_by_key(memc, LITERAL("master"),
                                          LITERAL("frog"), 0));
          REQUIRE(MEMCACHED_NOTFOUND ==
                  memcached_exist_by_key(memc, LITERAL("master"),
                                         LITERAL("frog")));
        }
      }
    }
  }
}
