#include "test/lib/common.hpp"
#include "test/lib/Shell.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

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

    SECTION("getpid2") {
      memcached_return_t rc;

      for (auto &server : test.cluster.getServers()) {
        REQUIRE(server.getPid() == libmemcached_util_getpid2("localhost", get<int>(server.getSocketOrPort()), "memcached", "memcached", &rc));
        REQUIRE_SUCCESS(rc);
      }

      REQUIRE(-1 == libmemcached_util_getpid2("localhost", 1, "foo", "bar", &rc));
      REQUIRE(memcached_fatal(rc));
    }

    SECTION("ping2") {
      memcached_server_fn fptr[] = {&ping2_callback};

      REQUIRE_SUCCESS(memcached_server_cursor(memc, fptr, nullptr, 1));
    }
  }
#endif
}
