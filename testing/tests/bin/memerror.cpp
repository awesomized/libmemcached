#include "testing/lib/common.hpp"
#include "testing/lib/Shell.hpp"
#include "testing/lib/Server.hpp"
#include "testing/lib/Retry.hpp"
#include "testing/lib/ReturnMatcher.hpp"

using Catch::Matchers::Contains;

TEST_CASE("bin/memerror") {
  Shell sh{string{TESTING_ROOT "/../src/bin"}};

  SECTION("--help") {
    string output;
    REQUIRE(sh.run("memerror --help", output));
    REQUIRE_THAT(output, Contains("memerror"));
    REQUIRE_THAT(output, Contains("v1"));
    REQUIRE_THAT(output, Contains("help"));
    REQUIRE_THAT(output, Contains("version"));
    REQUIRE_THAT(output, Contains("option"));
    REQUIRE_THAT(output, Contains("--"));
    REQUIRE_THAT(output, Contains("="));
  }

  SECTION("valid error codes") {
    for (underlying_type_t<memcached_return_t> rc = MEMCACHED_SUCCESS; rc < MEMCACHED_MAXIMUM_RETURN; ++rc) {
      string output;
      REQUIRE(sh.run("memerror " + to_string(rc), output));
      CHECK(output == memcached_strerror(nullptr, static_cast<memcached_return_t>(rc)) + string{"\n"});
    }
  }

  SECTION("unknown error code") {
    string output;
    REQUIRE_FALSE(sh.run("memerror " + to_string(MEMCACHED_MAXIMUM_RETURN), output));
    REQUIRE_THAT(output, Contains("INVALID memcached_return_t"));
  }
}
