#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

TEST_CASE("memcached_behavior") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;

  SECTION("IO_KEY_PREFETCH") {
    test.enableBinaryProto();
    constexpr int NUM_KEYS = 2048;

    // use batches of 64 keys
    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_IO_KEY_PREFETCH, 64));

    array<string, NUM_KEYS> str;
    array<char *, NUM_KEYS> chr;
    array<size_t, NUM_KEYS> len;

    for (auto i = 0; i < NUM_KEYS; ++i) {
      str[i] = to_string(i);
      chr[i] = str[i].data();
      len[i] = str[i].length();
      REQUIRE_SUCCESS(memcached_set(memc, chr[i], len[i], chr[i], len[i], 0, 0));
    }

    memcached_quit(memc);

    size_t counter = 0;
    memcached_execute_fn cb[] = {&callback_counter};
    REQUIRE_SUCCESS(memcached_mget_execute(memc, chr.data(), len.data(), NUM_KEYS, cb, &counter, 1));
    auto q_id = memcached_query_id(memc);
    REQUIRE_SUCCESS(memcached_fetch_execute(memc, cb, &counter, 1));
    REQUIRE(counter == NUM_KEYS);
    REQUIRE(q_id == memcached_query_id(memc));
  }
}
