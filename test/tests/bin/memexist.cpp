#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memexist") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memexist", output));
    REQUIRE(output == "No servers provided.\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memexist --help", output));
    REQUIRE_THAT(output, Contains("memexist v1"));
    REQUIRE_THAT(output, Contains("Usage:"));
    REQUIRE_THAT(output, Contains("key [key ...]"));
    REQUIRE_THAT(output, Contains("Options:"));
    REQUIRE_THAT(output, Contains("-h|--help"));
    REQUIRE_THAT(output, Contains("-V|--version"));
    REQUIRE_THAT(output, Contains("Environment:"));
    REQUIRE_THAT(output, Contains("MEMCACHED_SERVERS"));
  }

  SECTION("with server") {
    Server server{MEMCACHED_BINARY, {"-p", random_port_string}};
    MemcachedPtr memc;
    LoneReturnMatcher test{*memc};

    REQUIRE(server.ensureListening());
    auto port = get<int>(server.getSocketOrPort());
    auto comm = "memexist --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("found") {
      REQUIRE_SUCCESS(memcached_set(*memc, S("memexist"), S("MEMEXIST-SET"), 0, 0));

      string output;
      REQUIRE(sh.run(comm + "memexist", output));
      REQUIRE(output.empty());
    }

    SECTION("not found") {
      memcached_delete(*memc, S("memexist"), 0);

      string output;
      REQUIRE_FALSE(sh.run(comm + "memexist", output));
      REQUIRE(output.empty());
    }
  }
}
