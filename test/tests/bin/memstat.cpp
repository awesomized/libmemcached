#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memstat") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memstat", output));
    REQUIRE(output == "No Servers provided\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memstat --help", output));
    REQUIRE_THAT(output, Contains("memstat"));
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
    auto comm = "memstat --servers=localhost:" + to_string(port)
        + ",localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("okay") {
      SECTION("version") {
        REQUIRE_SUCCESS(memcached_version(*memc));
        auto inst = memcached_server_instance_by_position(*memc, 0);

        string output;
        REQUIRE(sh.run(comm + "--server-version", output));
        REQUIRE_THAT(output, Contains("localhost:" + to_string(port) + " "
            + to_string(memcached_server_major_version(inst)) + "."
            + to_string(memcached_server_minor_version(inst)) + "."
            + to_string(memcached_server_micro_version(inst))));
      }
      SECTION("analyze") {
        string output;
        REQUIRE(sh.run(comm + "--analyze", output));
        REQUIRE_THAT(output, Contains("Number of Servers Analyzed         : 2"));
      }
    }

    SECTION("connection failure") {
      server.signal(SIGKILL);
      server.wait();

      string output;
      REQUIRE_FALSE(sh.run(comm + "--analyze", output));
      REQUIRE_THAT(output,
              Contains("CONNECTION FAILURE")
          ||  Contains("SERVER HAS FAILED")
          ||  Contains("SYSTEM ERROR")
          ||  Contains("TIMEOUT OCCURRED"));
    }
  }
}
