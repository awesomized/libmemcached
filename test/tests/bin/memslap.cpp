#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/MemcachedCluster.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memslap") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memslap", output));
    REQUIRE(output == "No servers provided.\n");
  }

  SECTION("--help") {
    string output;

    REQUIRE(sh.run("memslap --help", output));
    REQUIRE_THAT(output, Contains("memslap v1"));
    REQUIRE_THAT(output, Contains("Usage:"));
    REQUIRE_THAT(output, Contains("Options:"));
    REQUIRE_THAT(output, Contains("-h|--help"));
    REQUIRE_THAT(output, Contains("-V|--version"));
    REQUIRE_THAT(output, Contains("--concurrency"));
    REQUIRE_THAT(output, Contains("Environment:"));
    REQUIRE_THAT(output, Contains("MEMCACHED_SERVERS"));
  }

  SECTION("with servers") {
    auto test = MemcachedCluster::udp();
    auto flags = {"--binary", "--udp", "--flush", "--test=mget", "--test=get", "--test=set",
                  "--tcp-nodelay", "--non-blocking", "--execute-number=1000"};
    string servers{"--servers="};
    auto verbosity = GENERATE(as<string>(), " --verbose ", " --quiet ");

    for (const auto &server : test.cluster.getServers()) {
      servers += "localhost:" + to_string(get<int>(server.getSocketOrPort())) + ", ";
    }

    for (const auto flag : flags) {
      string output;
      REQUIRE(sh.run("memslap --concurrency=2 " + servers + verbosity + flag, output));
      if (verbosity != " --quiet ") {
        REQUIRE_THAT(output, Contains("Starting"));
        REQUIRE_THAT(output, Contains("Time to"));
        REQUIRE_THAT(output, Contains("seconds"));
      } else {
        REQUIRE(output.empty());
      }
    }
  }
}
