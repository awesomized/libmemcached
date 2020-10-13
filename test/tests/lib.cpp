#include "test/lib/common.hpp"
#include "test/lib/Cluster.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/Server.hpp"

TEST_CASE("lib/Server") {
  Server server{MEMCACHED_BINARY, {Server::arg_t{"-v"}}};

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
  Cluster cluster{Server{MEMCACHED_BINARY, {
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
