#include "testing/lib/common.hpp"
#include "testing/lib/Shell.hpp"
#include "testing/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_sasl") {
#if !LIBMEMCACHED_WITH_SASL_SUPPORT
  WARN("ENABLE_SASL=OFF");
#else
  Shell sh;
  string mc{MEMCACHED_BINARY}, err;

  if (!sh.run(mc + " -S --version", err)) {
    WARN(mc << ": " << err);
  } else {
    auto test = MemcachedCluster::sasl();
    auto memc = &test.memc;

    REQUIRE_SUCCESS(memcached_set(memc, S(__func__), S(__func__), 0, 0));
    REQUIRE_SUCCESS(memcached_delete(memc, S(__func__), 0));
    REQUIRE_SUCCESS(memcached_destroy_sasl_auth_data(memc));
    REQUIRE_SUCCESS(memcached_destroy_sasl_auth_data(memc));
    REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS, memcached_destroy_sasl_auth_data(nullptr));

    memcached_quit(memc);

    REQUIRE_RC(MEMCACHED_AUTH_FAILURE, memcached_set(memc, S(__func__), S(__func__), 0, 0));

    REQUIRE_SUCCESS(memcached_set_sasl_auth_data(memc, "username", "password"));
    REQUIRE_RC(MEMCACHED_AUTH_FAILURE, memcached_set(memc, S(__func__), S(__func__), 0, 0));
  }
#endif
}
