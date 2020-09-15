#include "testing/lib/common.hpp"
#include "testing/lib/Shell.hpp"
#include "testing/lib/Server.hpp"
#include "testing/lib/Retry.hpp"
#include "testing/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memflush") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memflush", output));
    REQUIRE(output == "No Servers provided\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memflush --help", output));
    REQUIRE_THAT(output, Contains("memflush"));
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
      REQUIRE_THAT(output, Contains("CONNECTION FAILURE") || Contains("SERVER HAS FAILED"));
    }
  }
}
