#include "testing/lib/common.hpp"
#include "testing/lib/Shell.hpp"
#include "testing/lib/Server.hpp"
#include "testing/lib/Retry.hpp"
#include "testing/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memexist") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memexist", output));
    REQUIRE(output == "No Servers provided\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memexist --help", output));
    REQUIRE_THAT(output, Contains("memexist"));
    REQUIRE_THAT(output, Contains("v1"));
    REQUIRE_THAT(output, Contains("help"));
    REQUIRE_THAT(output, Contains("version"));
    REQUIRE_THAT(output, Contains("option"));
    REQUIRE_THAT(output, Contains("--"));
    REQUIRE_THAT(output, Contains("="));
  }

  SECTION("with server") {
    Server server{MEMCACHED_BINARY, {"-p", random_port_string}};
    MemcachedPtr memc;
    LoneReturnMatcher test{*memc};

    server.start();
    Retry{[&server] { return server.isListening(); }}();
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
