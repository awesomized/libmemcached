#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_gat") {
  auto test = MemcachedCluster::mixed();
  auto memc = &test.memc;
  memcached_return_t rc;
  auto binary = GENERATE(0, 1);

  test.enableBinaryProto(binary);

  DYNAMIC_SECTION("gat binary=" << binary) {
    REQUIRE_FALSE(memcached_gat(memc, S("gat_missing"), 60, nullptr, nullptr, &rc));
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);

    REQUIRE_SUCCESS(memcached_set(memc, S("gat_key"), S("hello"), 60, 42));
    size_t len;
    uint32_t flags;
    Malloced val(memcached_gat(memc, S("gat_key"), 120, &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(string("hello") == string(*val, len));
    REQUIRE(flags == 42);

    Malloced val_get(memcached_get(memc, S("gat_key"), &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val_get);
    REQUIRE(string("hello") == string(*val_get, len));
    REQUIRE(flags == 42);

    /* GAT with a past timestamp causes the key to expire immediately; the
       value is still returned for this call (server touches then returns) */
    Malloced val_expire(memcached_gat(memc, S("gat_key"), time(nullptr) - 2, &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val_expire);
    REQUIRE(string("hello") == string(*val_expire, len));
    REQUIRE(flags == 42);

    /* Key is now expired */
    Malloced val_expired(memcached_get(memc, S("gat_key"), nullptr, nullptr, &rc));
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
    REQUIRE_FALSE(*val_expired);

    /* noreply, gat always gets a reply */
    REQUIRE_SUCCESS(memcached_set(memc, S("gat_key"), S("hello"), 60, 42));
    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NOREPLY, true));
    Malloced val_gat_no_reply(memcached_gat(memc, S("gat_key"), 60, &len, nullptr, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(string("hello") == string(*val_gat_no_reply, len));
  }

  DYNAMIC_SECTION("gat_by_key binary=" << binary) {
    uint32_t flags;
    Malloced val_miss(memcached_gat_by_key(memc, S("group"), S("gat_by_key"), 60,
                                           nullptr, nullptr, &rc));
    REQUIRE_FALSE(*val_miss);
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);

    REQUIRE_SUCCESS(
        memcached_set_by_key(memc, S("group"), S("gat_by_key"), S("world"), 60, 42));

    size_t len;
    Malloced val(memcached_gat_by_key(memc, S("group"), S("gat_by_key"), 120,
                                       &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val);
    REQUIRE(string("world") == string(*val, len));
    REQUIRE(flags == 42);

    Malloced val_get(memcached_get_by_key(memc, S("group"), S("gat_by_key"),
                                       &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val_get);
    REQUIRE(string("world") == string(*val_get, len));
    REQUIRE(flags == 42);

    /* GAT with a past timestamp causes the key to expire immediately; the
       value is still returned for this call (server touches then returns) */
    Malloced val_expire(memcached_gat_by_key(memc, S("group"), S("gat_by_key"), time(nullptr) - 2, &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(*val_expire);
    REQUIRE(string("world") == string(*val_expire, len));
    REQUIRE(flags == 42);

    /* Key is now expired */
    Malloced val_expired(memcached_get_by_key(memc, S("group"), S("gat_by_key"),
                                        nullptr, nullptr, &rc));
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);
    REQUIRE_FALSE(*val_expired);

    /* noreply, gat always gets a reply */
    REQUIRE_SUCCESS(
        memcached_set_by_key(memc, S("group"), S("gat_by_key"), S("world"), 60, 42));
    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NOREPLY, true));
    Malloced val_gat_no_reply(memcached_gat_by_key(memc, S("group"), S("gat_by_key"), 60, &len, &flags, &rc));
    REQUIRE_SUCCESS(rc);
    REQUIRE(string("world") == string(*val_gat_no_reply, len));
  }
}
