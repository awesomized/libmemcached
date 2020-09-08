#include "testing/lib/common.hpp"
#include "testing/lib/Cluster.hpp"
#include "testing/lib/Retry.hpp"
#include "testing/lib/Server.hpp"

TEST_CASE("lib/Server") {
  Server server{"memcached"};

  SECTION("starts and listens") {

    REQUIRE(server.start().has_value());

    Retry server_is_listening{[&server] {
      return server.isListening();
    }};
    REQUIRE(server_is_listening());

    SECTION("stops") {

      REQUIRE(server.stop());

      SECTION("is waitable") {

        REQUIRE(server.wait());

        SECTION("stopped") {

          REQUIRE_FALSE(server.check());
        }
      }
    }
  }
}

TEST_CASE("lib/Cluster") {
  Cluster cluster{Server{"memcached", {
    random_socket_or_port_arg(),
  }}};

  SECTION("starts and listens") {

    REQUIRE(cluster.start());

    Retry cluster_is_listening{[&cluster] {
      return cluster.isListening();
    }};
    REQUIRE(cluster_is_listening());

    SECTION("stops") {

      cluster.stop();
      cluster.wait();

      SECTION("stopped") {

        REQUIRE(cluster.isStopped());
      }
    }
  }
}
