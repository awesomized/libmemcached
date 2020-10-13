#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/Server.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memcapable") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memcapable", output));
    REQUIRE(output == "No hostname was provided.\n");
  }

  SECTION("--help") {
    string output;

    REQUIRE(sh.run("memcapable --help", output));
    REQUIRE_THAT(output, Contains("memcapable") && Contains("binary") && Contains("ascii"));
  }

  SECTION("with server") {
    Server server{MEMCACHED_BINARY, {"-p", random_port_string}};

    REQUIRE(server.ensureListening());

    SECTION("ascii only") {
      string output;
      REQUIRE(sh.run("memcapable -a -h localhost -p " + to_string(get<int>(server.getSocketOrPort())), output));
      REQUIRE_THAT(output, Contains("pass") && Contains("ascii"));
      REQUIRE_THAT(output, !Contains("fail") && !Contains("binary"));
    }
    SECTION("binary only") {
      string output;
      REQUIRE(sh.run("memcapable -b -h localhost -p " + to_string(get<int>(server.getSocketOrPort())), output));
      REQUIRE_THAT(output, Contains("pass") && Contains("binary"));
      REQUIRE_THAT(output, !Contains("fail") && !Contains("ascii"));
    }
    SECTION("ascii and binary") {
      string output;
      REQUIRE(sh.run("memcapable -h localhost -p " + to_string(get<int>(server.getSocketOrPort())), output));
      REQUIRE_THAT(output, Contains("pass") && Contains("ascii") && Contains("binary"));
      REQUIRE_THAT(output, !Contains("fail"));
    }

    // fast exit
    server.signal(SIGKILL);
  }
}
