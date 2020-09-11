#include "testing/lib/common.hpp"
#include "testing/lib/Shell.hpp"
#include "testing/lib/Server.hpp"
#include "testing/lib/Retry.hpp"
#include "testing/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("memcat") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memcat", output));
    REQUIRE(output == "No servers provided\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memcat --help", output));
    REQUIRE_THAT(output, Contains("memcat"));
    REQUIRE_THAT(output, Contains("v1"));
    REQUIRE_THAT(output, Contains("help"));
    REQUIRE_THAT(output, Contains("version"));
    REQUIRE_THAT(output, Contains("option"));
    REQUIRE_THAT(output, Contains("--"));
    REQUIRE_THAT(output, Contains("="));
  }

  SECTION("with server") {
    Server server{"memcached"};
    MemcachedPtr memc;
    LoneReturnMatcher test{*memc};

    server.start();
    Retry{[&server] { return server.isListening(); }}();
    auto port = get<int>(server.getSocketOrPort());
    auto comm = "memcat --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("found") {
      REQUIRE_SUCCESS(memcached_set(*memc, S("memcat"), S("MEMCAT-SET"), 0, 0));

      string output;
      REQUIRE(sh.run(comm + "memcat", output));
      REQUIRE(output == "MEMCAT-SET\n");
    }

    SECTION("not found") {
      memcached_delete(*memc, S("memcat"), 0);

      string output;
      REQUIRE_FALSE(sh.run(comm + "memcat", output));
      REQUIRE_THAT(output, !Contains("MEMCAT-SET"));
      REQUIRE_THAT(output, Contains("NOT FOUND"));
    }
  }
}
