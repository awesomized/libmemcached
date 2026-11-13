#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_gat") {
  auto test = MemcachedCluster::mixed();
  auto memc = &test.memc;
  memcached_return_t rc;
  auto binary = GENERATE(0, 1);

  test.enableBinaryProto(binary);

  DYNAMIC_SECTION("gat missing key binary=" << binary) {
    REQUIRE_FALSE(memcached_gat(memc, S("gat_missing"), 60, nullptr, nullptr, &rc));
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
  }

  DYNAMIC_SECTION("gat returns value binary=" << binary) {
    REQUIRE_SUCCESS(memcached_set(memc, S("gat_key"), S("hello"), 60, 0));

    size_t len;
    uint32_t flags;
    Malloced val(memcached_gat(memc, S("gat_key"), 120, &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(string("hello") == string(*val, len));
    REQUIRE(flags == 0);
  }

  DYNAMIC_SECTION("gat extends expiration binary=" << binary) {
    /* Set with short TTL then extend via gat */
    REQUIRE_SUCCESS(memcached_set(memc, S("gat_extend"), S("value"), 2, 0));

    Malloced val(memcached_gat(memc, S("gat_extend"), 60, nullptr, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);

    /* Wait past the original 2s TTL to prove gat actually extended it */
    this_thread::sleep_for(3s);

    Malloced val2(memcached_get(memc, S("gat_extend"), nullptr, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val2);
  }

  DYNAMIC_SECTION("gat expires key immediately binary=" << binary) {
    REQUIRE_SUCCESS(memcached_set(memc, S("gat_expire"), S("soon"), 60, 0));

    /* GAT with a past timestamp causes the key to expire immediately; the
       value is still returned for this call (server touches then returns) */
    Malloced val(memcached_gat(memc, S("gat_expire"), time(nullptr) - 2, nullptr, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);

    /* Key is now expired */
    Malloced val2(memcached_get(memc, S("gat_expire"), nullptr, nullptr, &rc));
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
    REQUIRE_FALSE(*val2);
  }

  DYNAMIC_SECTION("gat preserves flags binary=" << binary) {
    REQUIRE_SUCCESS(memcached_set(memc, S("gat_flags"), S("flagged"), 60, 42));

    uint32_t flags;
    Malloced val(memcached_gat(memc, S("gat_flags"), 60, nullptr, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(flags == 42);
  }

  DYNAMIC_SECTION("gat_by_key missing key binary=" << binary) {
    Malloced val(memcached_gat_by_key(memc, S("group"), S("gat_by_key_missing"), 60,
                                       nullptr, nullptr, &rc));
    REQUIRE_FALSE(*val);
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
  }

  DYNAMIC_SECTION("gat_by_key returns value binary=" << binary) {
    REQUIRE_SUCCESS(
        memcached_set_by_key(memc, S("group"), S("gat_by_key_val"), S("world"), 60, 0));

    size_t len;
    Malloced val(memcached_gat_by_key(memc, S("group"), S("gat_by_key_val"), 120,
                                       &len, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(string("world") == string(*val, len));
  }

  DYNAMIC_SECTION("gat_by_key extends expiration binary=" << binary) {
    REQUIRE_SUCCESS(
        memcached_set_by_key(memc, S("group"), S("gat_by_key_ext"), S("extend"), 2, 0));

    Malloced val(memcached_gat_by_key(memc, S("group"), S("gat_by_key_ext"), 60,
                                       nullptr, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);

    /* Wait past the original 2s TTL to prove gat_by_key actually extended it */
    this_thread::sleep_for(3s);

    Malloced val2(memcached_get_by_key(memc, S("group"), S("gat_by_key_ext"),
                                        nullptr, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val2);
  }
}
