#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

#define NUM_KEYS 20480U

TEST_CASE("memcached_regression_lp490486") {
  MemcachedCluster test{Cluster{Server{MEMCACHED_BINARY, {"-p", random_port_string}}, 1}};
  auto memc = &test.memc;

  test.enableBinaryProto();

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 1000));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 3600));

  array<string, NUM_KEYS> str;
  array<char *, NUM_KEYS> chr;
  array<size_t, NUM_KEYS> len;

  for (auto i = 0U; i < NUM_KEYS; ++i) {
    char blob[1024];

    str[i] = random_ascii_string(12) + to_string(i);
    chr[i] = str[i].data();
    len[i] = str[i].length();

    REQUIRE_SUCCESS(memcached_set(memc, chr[i], len[i], blob, sizeof(blob), 0, 0));
  }

  size_t counter = 0;
  memcached_return_t rc;
  memcached_execute_fn cb[] = {&callback_counter};
  REQUIRE_SUCCESS(memcached_mget_execute(memc, chr.data(), len.data(), NUM_KEYS, cb, &counter, 1));

  do {
    char key_buf[MEMCACHED_MAX_KEY];
    size_t key_len;

    Malloced value(memcached_fetch(memc, key_buf, &key_len, nullptr, nullptr, &rc));
    counter += !!*value;
  } while(rc == MEMCACHED_SUCCESS);

  REQUIRE_RC(MEMCACHED_END, rc);
  REQUIRE(counter == NUM_KEYS);
}
