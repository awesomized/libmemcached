#include "lib/catch.hpp"
#include "lib/Cluster.hpp"

#include "lib/random_.hpp"

TEST_CASE("Server") {
  Server server{"memcached"};

  SECTION("starts and listens") {

    REQUIRE(server.start().has_value());
    REQUIRE(server.isListening());

    SECTION("stops") {

      REQUIRE(server.stop());

      SECTION("is waitable") {

        REQUIRE(server.wait());

        SECTION("stopped") {
          REQUIRE(server.is)
        }
      }
    }
  }
}

TEST_CASE("Cluster") {
  Cluster cluster{Server{"memcached", {
    random_socket_or_port_arg(),
  }}};

  SECTION("starts and listens") {

    REQUIRE(cluster.start());
    REQUIRE(cluster.isListening());

    SECTION("stops") {

      cluster.stop();
      cluster.wait();

      SECTION("stopped") {

        REQUIRE(cluster.isStopped());
      }
    }
  }
}
