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
    REQUIRE(output == "No servers provided.\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memstat --help", output));
    REQUIRE_THAT(output, Contains("memstat v1"));
    REQUIRE_THAT(output, Contains("Usage:"));
    REQUIRE_THAT(output, Contains("[stat ...]"));
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
    auto comm = "memstat --servers=localhost:" + to_string(port)
        + ",localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("okay") {
      SECTION("stat") {
        string output;
        REQUIRE(sh.run(comm, output));
        REQUIRE_THAT(output, Contains("Server:"));
        REQUIRE_THAT(output, Contains("pid:"));
      }
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
      SECTION("analyze=latency") {
        string output;
        REQUIRE(sh.run(comm + "--analyze=latency", output));
        REQUIRE_THAT(output, Contains("Network Latency Test:"));
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
