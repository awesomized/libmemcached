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
    REQUIRE(output == "No servers provided.\n");
  }

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memcp --help", output));
    REQUIRE_THAT(output, Contains("memcp v1"));
    REQUIRE_THAT(output, Contains("Usage:"));
    REQUIRE_THAT(output, Contains("file [file ...]"));
    REQUIRE_THAT(output, Contains("Options:"));
    REQUIRE_THAT(output, Contains("-h|--help"));
    REQUIRE_THAT(output, Contains("-V|--version"));
    REQUIRE_THAT(output, Contains("Environment:"));
    REQUIRE_THAT(output, Contains("MEMCACHED_SERVERS"));
  }

  SECTION("with server") {
    Server server{MEMCACHED_BINARY, {"-U", random_port_string}};
    MemcachedPtr memc;
    LoneReturnMatcher test{*memc};

    REQUIRE(server.ensureListening());
    auto port = get<int>(server.getSocketOrPort());
    auto comm = "memcp --servers=localhost:" + to_string(port) + " ";

    REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", port));

    SECTION("okay") {
      auto udp_buffer = GENERATE(0,1,2);
      auto binary = GENERATE(0,1);
      auto set_add_replace = GENERATE(0,1,2);
      auto expire = GENERATE(0, random_num(10,12345));
      string set_add_replace_s[3] = {
          "set", "add", "replace"
      };

      DYNAMIC_SECTION("udp=" << (udp_buffer==1) <<" buffer=" << (udp_buffer==2) << " binary=" << binary <<  " mode=" << set_add_replace_s[set_add_replace] << " expire=" << expire) {
        Tempfile temp;
        temp.put(S("123"));

        if (udp_buffer == 1) {
          comm += " --udp ";
        } else if (udp_buffer == 2) {
          comm += " --buffer ";
        }
        if(binary) {
          comm += " --binary ";
        }
        if (expire) {
          comm += " --expire " + to_string(expire) + " ";
        }
        switch (set_add_replace) {
        case 2:
          comm += " --replace ";
          REQUIRE_SUCCESS(memcached_set(*memc, S(temp.getFn()), S("foo"), 0, 0));
          break;
        case 1:
          comm += " --add ";
        }

        INFO(comm);
        string output;
        auto ok = sh.run(comm + temp.getFn(), output);
        REQUIRE(output == "");
        REQUIRE(ok);

        Retry settled{[&memc, &temp]{
          size_t len;
          memcached_return_t rc;
          Malloced val(memcached_get(*memc, S(temp.getFn()), &len, nullptr, &rc));

          return MEMCACHED_SUCCESS == rc && *val && string(*val, len) == "123";
        }};
        REQUIRE(settled());
      }
    }

    SECTION("connection failure") {
      server.signal(SIGKILL);
      server.wait();

      Tempfile temp;

      string output;
      REQUIRE_FALSE(sh.run(comm + temp.getFn(), output));
      REQUIRE_THAT(output,
              Contains("CONNECTION FAILURE")
          ||  Contains("SERVER HAS FAILED")
          ||  Contains("SYSTEM ERROR")
          ||  Contains("TIMEOUT OCCURRED"));
    }

    SECTION("file not found") {
      string output;
      REQUIRE_FALSE(sh.run(comm + "nonexistent", output));
      REQUIRE_THAT(output, Contains("No such file or directory"));
    }
  }
}
