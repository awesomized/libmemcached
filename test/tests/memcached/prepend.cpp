#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_prepend") {
  pair<string, MemcachedCluster> tests[] = {
      {"network", MemcachedCluster::network()},
      {"socket", MemcachedCluster::socket()}
  };

  LOOPED_SECTION(tests) {
    auto memc = &test.memc;

    test.flush();

    REQUIRE_RC(MEMCACHED_NOTSTORED, memcached_prepend(memc, S(__func__), S("fail"), 0, 0));
    REQUIRE_SUCCESS(memcached_set(memc, S(__func__), S("initial"), 0, 0));
    REQUIRE_SUCCESS(memcached_prepend(memc, S(__func__), S("pre1"), 0, 0));
    REQUIRE_SUCCESS(memcached_prepend(memc, S(__func__), S("pre2"), 0, 0));

    memcached_return_t rc;
    uint32_t flags;
    size_t len;
    char *val = memcached_get(memc, S(__func__), &len, &flags, &rc);
    Malloced v(val);
    REQUIRE_SUCCESS(rc);
    REQUIRE("pre2pre1initial" == string(val, len));
  }
}
