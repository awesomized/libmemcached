#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

TEST_CASE("memcached_regression_lp1630615") {
#if !LIBMEMCACHED_WITH_SASL_SUPPORT
  WARN("ENABLE_SASL=OFF");
#else
  Shell sh;
  string mc{MEMCACHED_BINARY}, err;

  if (!sh.run(mc + " -S --version", err)) {
    WARN(mc << ": " << err);
  } else {
    memcached_return_t rc;
    auto test = MemcachedCluster::sasl();
    auto memc = &test.memc;
    MemcachedPtr copy{memcached_clone(nullptr, memc)};

    REQUIRE(memcached_server_by_key(memc, S(__func__), &rc)->fd == INVALID_SOCKET);
    REQUIRE_SUCCESS(memcached_set(memc, S(__func__), S(__func__), 0, 0));
    REQUIRE(memcached_server_by_key(memc, S(__func__), &rc)->fd != INVALID_SOCKET);

    REQUIRE(memcached_server_by_key(*copy, S(__func__), &rc)->fd == INVALID_SOCKET);
    Malloced val{memcached_get(*copy, S(__func__), nullptr, nullptr, &rc)};
    REQUIRE(memcached_server_by_key(*copy, S(__func__), &rc)->fd != INVALID_SOCKET);
    
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(string(__func__) == string(*val));
  }
#endif
}
