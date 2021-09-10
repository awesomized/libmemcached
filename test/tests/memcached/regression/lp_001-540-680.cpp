#include "test/lib/common.hpp"
#include "test/lib/random.hpp"
#include "test/lib/Server.hpp"
#include "test/lib/ReturnMatcher.hpp"

TEST_CASE("memcached_regression_lp1540680") {
  Server timeout{TESTING_ROOT "/timeout", {"-p", random_port_string}};
  MemcachedPtr memc;
  LoneReturnMatcher test{*memc};

  REQUIRE(timeout.start());
  this_thread::sleep_for(500ms);

  REQUIRE_SUCCESS(memcached_server_add(*memc, "localhost", get<int>(timeout.getSocketOrPort())));

  memcached_return_t rc;
  Malloced val(memcached_get(*memc, S("not-found"), nullptr, nullptr, &rc));
  REQUIRE_RC(MEMCACHED_TIMEOUT, rc);
  REQUIRE_FALSE(*val);
}
