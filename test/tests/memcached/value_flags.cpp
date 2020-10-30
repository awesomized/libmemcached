#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_value_flags") {
  auto test = MemcachedCluster::mixed();
  auto memc = &test.memc;
  const auto size = 1048064lu;
  auto blob = make_unique<char[]>(size);

  SECTION("set & get flags") {
    uint32_t flag = GENERATE(123, 456, 789, UINT32_MAX);

    REQUIRE_SUCCESS(memcached_set(memc, S(__func__), blob.get(), size, 0, flag));

    size_t len;
    uint32_t flg;
    memcached_return_t rc;
    Malloced val(memcached_get(memc, S(__func__), &len, &flg, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(len == size);
    REQUIRE(flg == flag);

    len = strlen(__func__);
    auto fun = __func__;
    char key[MEMCACHED_MAX_KEY];
    memcpy(key, fun, strlen(fun) + 1);
    const char *keys[1] = {key};
    REQUIRE_SUCCESS(memcached_mget(memc, keys, &len, 1));
    while (auto rv = memcached_fetch(memc, key, nullptr, &len, &flg, &rc)) {
      REQUIRE_SUCCESS(rc);
      REQUIRE(len == size);
      REQUIRE_FALSE(memcmp(rv, blob.get(), len));
      REQUIRE(flag == flg);
      free(rv);
    }
    REQUIRE_RC(MEMCACHED_END, rc);
  }
}
