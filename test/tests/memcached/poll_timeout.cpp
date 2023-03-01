#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_poll_timeout") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, 0));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 20));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, 20*1000));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SND_TIMEOUT, 20*1000));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 60));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 20));


  memcached_return_t rc;
  size_t len;

  auto num = 500;
  auto hit = 0u;
  auto rnd = random_ascii_string(128);

  REQUIRE_SUCCESS(memcached_flush(memc, 0));

  for (auto i = 0; i < num; ++i) {
    auto setKey = rnd + to_string(i);
    REQUIRE_SUCCESS(memcached_set(memc, setKey.c_str(), setKey.length(), setKey.c_str(),
                                  setKey.length(), 1, 0));
  }
  memcached_quit(memc);

  auto timeoutAchieved = false;
  for (auto i = 0; i < num; ++i) {
    for (auto j = 0; j < i; ++j) {
      auto getKey = rnd + to_string(j);
      auto len = getKey.length();
      char key[MEMCACHED_MAX_KEY];
      memcpy(key, getKey.c_str(), len + 1);
      const char *keys[1] = {key};
      std::cerr << int(timeoutAchieved);

      REQUIRE_SUCCESS(memcached_mget(memc, keys, &len, 1));
      auto numResults = 0;
      while (auto rv = memcached_fetch(memc, key, nullptr, &len, nullptr, &rc)) {
        REQUIRE(!timeoutAchieved);

        REQUIRE_SUCCESS(rc);
        free(rv);

        numResults++;
        REQUIRE(numResults == 1);
      }

      if (MEMCACHED_END == rc || MEMCACHED_NOTFOUND == rc ){
        continue;
      }

      if (MEMCACHED_TIMEOUT == rc){
        timeoutAchieved = true;
        continue;
      }

      INFO(rc);
      REQUIRE(false);
    }
  }
}
