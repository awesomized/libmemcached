#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/lib/Retry.hpp"

TEST_CASE("memcached_errors") {
  SECTION("NO_SERVERS") {
    MemcachedPtr memc;
    memcached_return_t rc;
    auto key = "key";
    size_t len = 3;

    REQUIRE(MEMCACHED_NO_SERVERS == memcached_flush(*memc, 0));
    REQUIRE(MEMCACHED_NO_SERVERS == memcached_set(*memc, S(__func__), S(__func__), 0, 0));
    REQUIRE_FALSE(memcached_get(*memc, S(__func__), nullptr, nullptr, &rc));
    REQUIRE(MEMCACHED_NO_SERVERS == rc);
    REQUIRE(MEMCACHED_NO_SERVERS == memcached_mget(*memc, &key, &len, 1));
  }

  SECTION("dead servers") {
    MemcachedCluster test{Cluster{Server{MEMCACHED_BINARY, {"-p", random_port_string("-p")}}, 1}};
    auto memc = &test.memc;

    REQUIRE_SUCCESS(memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
    memcached_quit(memc);

    test.cluster.stop();
    Retry cluster_is_stopped{[&cluster = test.cluster]{
      return cluster.isStopped();
    }};
    REQUIRE(cluster_is_stopped());

    SECTION("TEMPORARILY_DISABLED") {
      REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 3));
      REQUIRE_RC(MEMCACHED_CONNECTION_FAILURE, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
      REQUIRE_RC(MEMCACHED_SERVER_TEMPORARILY_DISABLED, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
    }

    SECTION("recovers from TEMPORARILY_DISABLED") {
      REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 1));
      REQUIRE_RC(MEMCACHED_CONNECTION_FAILURE, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
      REQUIRE_RC(MEMCACHED_SERVER_TEMPORARILY_DISABLED, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));

      REQUIRE(test.cluster.start());
      REQUIRE(test.cluster.ensureListening());

      Retry recovers{[memc]{
        return MEMCACHED_SUCCESS == memcached_set(memc, S("foo"), nullptr, 0, 0, 0);
      }, 50, 100ms};
      REQUIRE(recovers());
    }

    SECTION("MARKED_DEAD") {
      SECTION("immediately") {
        REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, true));
        REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 1));

        REQUIRE_RC(MEMCACHED_CONNECTION_FAILURE, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
        REQUIRE_RC(MEMCACHED_SERVER_MARKED_DEAD, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
      }
      SECTION("with retry") {
        REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, true));
        REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 2));
        REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 1));

        REQUIRE_RC(MEMCACHED_CONNECTION_FAILURE, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));
        REQUIRE_RC(MEMCACHED_SERVER_TEMPORARILY_DISABLED, memcached_set(memc, S("foo"), nullptr, 0, 0, 0));

        Retry server_is_marked_dead{[memc] {
          return MEMCACHED_SERVER_MARKED_DEAD == memcached_set(memc, S("foo"), nullptr, 0, 0, 0);
        },50, 100ms};
        REQUIRE(server_is_marked_dead());
      }
    }
  }
}
