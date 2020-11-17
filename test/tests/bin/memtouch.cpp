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
    REQUIRE(output == "No servers provided.\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memtouch --help", output));
    REQUIRE_THAT(output, Contains("memtouch v1"));
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
      string output;
      bool ok = sh.run(comm + " -v --expire=" + to_string(time(nullptr) - 2) + " memtouch", output);
      REQUIRE(output == "");
      REQUIRE(ok);
      REQUIRE_RC(MEMCACHED_NOTFOUND, memcached_exist(*memc, S("memtouch")));
    }
  }
}
