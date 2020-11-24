#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/MemcachedCluster.hpp"

#if HAVE_MEMASLAP

using Catch::Matchers::Contains;

TEST_CASE("bin/memaslap") {
  Shell sh{string{TESTING_ROOT "/../contrib/bin/memaslap"}};

  SECTION("no servers provided") {
    string output;
    REQUIRE_FALSE(sh.run("memaslap", output));
    REQUIRE_THAT(output, Contains("No servers provided\n"));
  }

  SECTION("--help") {
    string output;

    REQUIRE(sh.run("memaslap --help", output));
    REQUIRE_THAT(output, Contains("memaslap"));
    REQUIRE_THAT(output, Contains("v1"));
    /* FIXME
    REQUIRE_THAT(output, Contains("help"));
    REQUIRE_THAT(output, Contains("version"));
    REQUIRE_THAT(output, Contains("option"));
     */
    REQUIRE_THAT(output, Contains("--"));
    REQUIRE_THAT(output, Contains("="));
  }

  SECTION("with servers") {
    auto test = MemcachedCluster::network();
    auto examples = {
        " -T 2 -c 8 -t 2s -S 1s",
        " -T 2 -c 8 -t 2s -v 0.2 -e 0.05 -b",
        " -T 2 -c 8 -t 2s -w 40k -o 0.2",
        " -T 2 -c 8 -t 2s -d 20 -P 40k",
#if 0
        " -T 2 -c 8 -t 2s -d 50 -a -n 10",
#endif
    };
    string servers{"-s "};

    for (const auto &server : test.cluster.getServers()) {
      servers += "127.0.0.1:" + to_string(get<int>(server.getSocketOrPort())) + ",";
    }
    for (const auto args : examples) {
      string output;
      INFO(args);
      auto ok = sh.run("memaslap -b " + servers + args, output);
      INFO(output);
      REQUIRE(ok);
      REQUIRE_THAT(output, Contains("TPS"));
    }
  }
}
#endif // HAVE_MEMASLAP
