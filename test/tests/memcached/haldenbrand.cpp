#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

/* Test case provided by Cal Haldenbrand */

#define HALDENBRAND_KEY_COUNT 3000U // * 1024576
#define HALDENBRAND_FLAG_KEY 99 // * 1024576

#include <cstdlib>

TEST_CASE("memcached_haldenbrand_nblock_tcp_ndelay") {
  pair<string, MemcachedCluster> tests[] = {
      {"network", MemcachedCluster::network()},
  };
  
  for (auto &[name, test] : tests) {
    REQUIRE_SUCCESS(memcached_behavior_set(&test.memc, MEMCACHED_BEHAVIOR_NO_BLOCK, true));
    REQUIRE_SUCCESS(memcached_behavior_set(&test.memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, true));
  }
  
  LOOPED_SECTION(tests) {
    auto memc = &test.memc;

    /* We just keep looking at the same values over and over */
    srandom(10);

    /* add key */
    unsigned long long total = 0;
    for (uint32_t x = 0; total < 20 * 1024576; x++) {
      uint32_t size = (uint32_t) (rand() % (5 * 1024)) + 400;
      char randomstuff[6 * 1024];
      memset(randomstuff, 0, 6 * 1024);
      REQUIRE(size < 6 * 1024); /* Being safe here */

      for (uint32_t j = 0; j < size; j++) {
        randomstuff[j] = (signed char) ((rand() % 26) + 97);
      }

      total += size;
      char key[MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH + 1];
      int key_length = snprintf(key, sizeof(key), "%u", x);
      REQUIRE_SUCCESS(memcached_set(memc, key, key_length,
          randomstuff, strlen(randomstuff),
          time_t(0), HALDENBRAND_FLAG_KEY
      ));
    }
    REQUIRE(total > HALDENBRAND_KEY_COUNT);

    size_t total_value_length = 0;
    for (uint32_t x = 0, errors = 0; total_value_length < 24576; x++) {
      uint32_t flags = 0;
      size_t val_len = 0;

      char key[MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH + 1];
      int key_length = snprintf(key, sizeof(key), "%u", x);

      memcached_return_t rc;
      char *getval = memcached_get(memc, key, key_length, &val_len, &flags, &rc);
      if (memcached_failed(rc)) {
        if (rc == MEMCACHED_NOTFOUND) {
          errors++;
        } else {
          REQUIRE(rc);
        }

        continue;
      }
      REQUIRE(uint32_t(HALDENBRAND_FLAG_KEY) == flags);
      REQUIRE(getval);

      total_value_length += val_len;
      errors = 0;
      ::free(getval);
    }


    std::vector<size_t> key_lengths;
    key_lengths.resize(HALDENBRAND_KEY_COUNT);
    std::vector<char *> keys;
    keys.resize(key_lengths.size());
    for (uint32_t x = 0; x < key_lengths.size(); x++) {
      char key[MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH + 1];
      int key_length = snprintf(key, sizeof(key), "%u", x);
      REQUIRE(key_length > 0);
      REQUIRE(key_length < MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH + 1);
      keys[x] = strdup(key);
      key_lengths[x] = key_length;
    }

    REQUIRE_SUCCESS(memcached_mget(memc, &keys[0], &key_lengths[0], key_lengths.size()));

    unsigned int keys_returned;
    REQUIRE(memcached_success(fetch_all_results(memc, keys_returned)));
    REQUIRE(HALDENBRAND_KEY_COUNT == keys_returned);

    for (auto key : keys) {
      free(key);
    }
  }
}
