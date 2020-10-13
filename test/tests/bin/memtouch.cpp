#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memtouch") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memtouch", output));
    REQUIRE(output == "No Servers provided\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memtouch --help", output));
    REQUIRE_THAT(output, Contains("memtouch"));
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
    auto comm = "memtouch --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("found") {
      REQUIRE_SUCCESS(memcached_set(*memc, S("memtouch"), S("memtouch-SET"), 0, 0));

      string output;
      REQUIRE(sh.run(comm + "memtouch", output));
      REQUIRE(output.empty());
    }

    SECTION("not found") {
      memcached_delete(*memc, S("memtouch"), 0);

      string output;
      REQUIRE_FALSE(sh.run(comm + "memtouch", output));
      REQUIRE(output.empty());
    }

    SECTION("expires") {
      REQUIRE_SUCCESS(memcached_set(*memc, S("memtouch"), S("memtouch"), 60, 0));
      REQUIRE_SUCCESS(memcached_exist(*memc, S("memtouch")));
      REQUIRE(sh.run(comm + "--expire=" + to_string(time(nullptr) - 2) + " memtouch"));
      REQUIRE_RC(MEMCACHED_NOTFOUND, memcached_exist(*memc, S("memtouch")));
    }
  }
}
