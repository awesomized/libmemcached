#include "../lib/common.hpp"

TEST_CASE("memcached servers") {
  SECTION("memcached_servers_parse") {
    SECTION("does not leak memory") {
      memcached_server_st *s = memcached_servers_parse("1.2.3.4:1234");
      REQUIRE(s);
      memcached_server_free(s);
    }
  }

  SECTION("memcached_server_list") {
    SECTION("append with weight - all zeros") {
      memcached_server_st *sl = memcached_server_list_append_with_weight(
          nullptr, nullptr, 0, 0, 0);
      REQUIRE(sl);
      memcached_server_list_free(sl);
    }
    SECTION("append with weight - host set only") {
      memcached_server_st *sl = memcached_server_list_append_with_weight(
          nullptr, "localhost", 0, 0, 0);
      REQUIRE(sl);
      memcached_server_list_free(sl);
    }
    SECTION("append with weight - error set only") {
      memcached_return_t rc;
      memcached_server_st *sl = memcached_server_list_append_with_weight(
          nullptr, nullptr, 0, 0, &rc);
      REQUIRE(sl);
      REQUIRE(MEMCACHED_SUCCESS == rc);
      memcached_server_list_free(sl);
    }
  }

  SECTION("no configured servers") {
    MemcachedPtr memc;

    REQUIRE(MEMCACHED_NO_SERVERS == memcached_increment(*memc, S("key"), 1, nullptr));
  }
}
