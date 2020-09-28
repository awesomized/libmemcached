#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/Retry.hpp"
#include "test/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memcp") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memcp nonexistent", output));
    REQUIRE(output == "No Servers provided\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memcp --help", output));
    REQUIRE_THAT(output, Contains("memcp"));
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
    auto comm = "memcp --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("okay") {
      Tempfile temp;
      temp.put(S("123"));

      string output;
      REQUIRE(sh.run(comm + temp.getFn(), output));
      REQUIRE(output == "");

      size_t len;
      memcached_return_t rc;
      Malloced val(memcached_get(*memc, S(temp.getFn()), &len, nullptr, &rc));

      REQUIRE(*val);
      REQUIRE_SUCCESS(rc);
      REQUIRE(string(*val, len) == "123");
    }

    SECTION("connection failure") {
      server.signal(SIGKILL);
      server.wait();

      Tempfile temp;

      string output;
      REQUIRE_FALSE(sh.run(comm + temp.getFn(), output));
      REQUIRE_THAT(output, Contains("CONNECTION FAILURE"));
    }

    SECTION("file not found") {
      string output;
      REQUIRE_FALSE(sh.run(comm + "nonexistent", output));
      REQUIRE_THAT(output, Contains("No such file or directory"));
    }
  }
}
