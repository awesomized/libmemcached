#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/MemcachedCluster.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memslap") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memslap", output));
    REQUIRE(output == "No servers provided\n");
  }

  SECTION("--help") {
    string output;

    REQUIRE(sh.run("memslap --help", output));
    REQUIRE_THAT(output, Contains("memslap"));
    REQUIRE_THAT(output, Contains("v1"));
    REQUIRE_THAT(output, Contains("help"));
    REQUIRE_THAT(output, Contains("version"));
    REQUIRE_THAT(output, Contains("option"));
    REQUIRE_THAT(output, Contains("--"));
    REQUIRE_THAT(output, Contains("="));
  }

  SECTION("with servers") {
    auto test = MemcachedCluster::udp();
    auto flags = {"--binary", "--udp", "--flush", "--test=mget", "--test=get", "--tcp-nodelay",
                  "--non-blocking", "--initial-load=1000"};
    string servers{"--servers="};

    for (const auto &server : test.cluster.getServers()) {
      servers += "localhost:" + to_string(get<int>(server.getSocketOrPort())) + ", ";
    }

    for (const auto flag : flags) {
      REQUIRE(sh.run("memslap --quiet --concurrency=2 " + servers + flag));
    }
  }
}
