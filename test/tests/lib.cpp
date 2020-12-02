#include "test/lib/common.hpp"
#include "test/lib/Cluster.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Connection.hpp"

TEST_CASE("lib/Server") {
  Server server{MEMCACHED_BINARY, {
    Server::arg_t{"-v"},
    Server::arg_pair_t{"-p", random_port_string}
  }};

  SECTION("starts and listens") {

    REQUIRE(server.start().has_value());
    REQUIRE(server.ensureListening());
    REQUIRE(server.isListening());
    REQUIRE(server.check());

    SECTION("stops") {

      REQUIRE(server.stop());

      SECTION("is waitable") {

        REQUIRE(server.wait());

        SECTION("stopped") {

          REQUIRE_FALSE(server.check());
          REQUIRE_FALSE(server.isListening());
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
    REQUIRE(cluster.ensureListening());
    REQUIRE(cluster.isListening());

    SECTION("stops") {

      cluster.stop();
      cluster.wait();

      SECTION("stopped") {

        REQUIRE(cluster.isStopped());
        REQUIRE_FALSE(cluster.isListening());
      }
    }
  }
}

TEST_CASE("lib/Connection") {
  SECTION("sockaddr_un") {
    auto f = []{
      Connection conn{"/this/is/way/too/long/for/a/standard/unix/socket/path/living/on/this/system/at/least/i/hope/so/and/this/is/about/to/fail/for/the/sake/of/this/test.sock"};
      return conn;
    };
    REQUIRE_THROWS(f());
  }
  SECTION("connect") {
    Cluster cluster{Server{MEMCACHED_BINARY,
                           {
                               random_socket_or_port_arg(),
                           }}};
    REQUIRE(cluster.start());
    Retry cluster_is_listening{[&cluster] { return cluster.isListening(); }};
    REQUIRE(cluster_is_listening());

    vector<Connection> conns;
    conns.reserve(cluster.getServers().size());
    for (const auto &server : cluster.getServers()) {
      CHECK_NOFAIL(conns.emplace_back(Connection{server.getSocketOrPort()}).open());
    }
    while (!conns.empty()) {
      vector<Connection> again;
      again.reserve(conns.size());
      for (auto &conn : conns) {
        if (conn.isOpen()) {
          REQUIRE(conn.isWritable());
          REQUIRE_FALSE(conn.getError());
        } else {
          again.emplace_back(move(conn));
        }
      }
      conns.swap(again);
    }
  }
}
