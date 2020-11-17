#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memflush") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memflush", output));
    REQUIRE(output == "No servers provided.\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memflush --help", output));
    REQUIRE_THAT(output, Contains("memflush v1"));
    REQUIRE_THAT(output, Contains("Usage:"));
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
    auto comm = "memflush --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("okay") {

      REQUIRE_SUCCESS(memcached_set(*memc, S("key1"), S("val1"), 0, 0));
      REQUIRE_SUCCESS(memcached_set(*memc, S("key2"), S("val2"), 0, 0));

      this_thread::sleep_for(500ms);

      string output;
      REQUIRE(sh.run(comm, output));
      REQUIRE(output.empty());

      memcached_return_t rc;
      REQUIRE(nullptr == memcached_get(*memc, S("key1"), nullptr, nullptr, &rc));
      REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
      REQUIRE(nullptr == memcached_get(*memc, S("key2"), nullptr, nullptr, &rc));
      REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
    }

    SECTION("connection failure") {
      server.signal(SIGKILL);
      server.wait();

      string output;
      REQUIRE_FALSE(sh.run(comm, output));
      REQUIRE_THAT(output,
              Contains("CONNECTION FAILURE")
          ||  Contains("SERVER HAS FAILED")
          ||  Contains("SYSTEM ERROR")
          ||  Contains("TIMEOUT OCCURRED"));
    }
  }
}
