#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memcat") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memcat", output));
    REQUIRE(output == "No servers provided\n");
  }

  SECTION("connection failure") {
    string output;
    CHECK_FALSE(sh.run("memcat --servers=localhost:" + random_port_string("-p") + " memcat", output));
    REQUIRE_THAT(output, Contains("CONNECTION FAILURE"));
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
    Server server{MEMCACHED_BINARY, {"-p", random_port_string}};
    MemcachedPtr memc;
    LoneReturnMatcher test{*memc};

    REQUIRE(server.ensureListening());
    auto port = get<int>(server.getSocketOrPort());
    auto comm = "memcat --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("not found") {
      memcached_delete(*memc, S("memcat"), 0);

      string output;
      REQUIRE_FALSE(sh.run(comm + "memcat", output));
      REQUIRE_THAT(output, !Contains("MEMCAT-SET"));
      REQUIRE_THAT(output, Contains("NOT FOUND"));
    }
    SECTION("found") {
      string output;
      REQUIRE_SUCCESS(memcached_set(*memc, S("memcat"), S("MEMCAT-SET"), 0, 123));

      SECTION("default") {
        REQUIRE(sh.run(comm + "memcat", output));
        REQUIRE(output == "MEMCAT-SET\n");
      }
      SECTION("flags") {
        REQUIRE(sh.run(comm + "--flag memcat", output));
        REQUIRE(output == "123\n");
        output.clear();
        REQUIRE(sh.run(comm + "--flag -v memcat", output));
        REQUIRE(output == "key: memcat\nflags: 123\n");
      }
      SECTION("file") {
        Tempfile temp;
        REQUIRE(sh.run(comm + "--file " + temp.getFn() + " memcat", output));
        REQUIRE(output.empty());
        REQUIRE(temp.get() == "MEMCAT-SET");
      }
    }
  }
}
