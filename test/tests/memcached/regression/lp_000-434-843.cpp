#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

#define NUM_KEYS 1024

TEST_CASE("memcached_regression_lp434843") {
  MemcachedCluster test{Cluster{Server{MEMCACHED_BINARY, {"-p", random_port_string}}, 1}};
  auto memc = &test.memc;
  auto buffering = GENERATE(0, 1);

  test.enableBinaryProto();
  test.enableBuffering(buffering);

  INFO("buffering: " << buffering);

  size_t counter = 0;
  memcached_execute_fn cb[] = {&callback_counter};

  array<string, NUM_KEYS> str;
  array<char *, NUM_KEYS> chr;
  array<size_t, NUM_KEYS> len;

  for (auto i = 0; i < NUM_KEYS; ++i) {
    str[i] = random_ascii_string(12) + to_string(i);
    chr[i] = str[i].data();
    len[i] = str[i].length();
  }

  REQUIRE_SUCCESS(memcached_mget(memc, chr.data(), len.data(), NUM_KEYS));
  REQUIRE_RC(MEMCACHED_NOTFOUND, memcached_fetch_execute(memc, cb, &counter, 1));
  REQUIRE(counter == 0);

  for (auto i = 0; i < NUM_KEYS; ++i) {
    char data[1024];
    REQUIRE_SUCCESS(memcached_add(memc, chr[i], len[i], data, sizeof(data), 0, 0));
  }

  REQUIRE_SUCCESS(memcached_mget(memc, chr.data(), len.data(), NUM_KEYS));
  REQUIRE_SUCCESS( memcached_fetch_execute(memc, cb, &counter, 1));
  REQUIRE(counter == NUM_KEYS);
}
