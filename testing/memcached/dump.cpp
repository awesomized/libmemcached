#include "../lib/common.hpp"
#include "../lib/MemcachedCluster.hpp"

memcached_return_t dump_cb(const memcached_st *, const char *, size_t, void *ctx) {
  size_t *c = reinterpret_cast<size_t *>(ctx);
  ++(*c);
  return MEMCACHED_SUCCESS;
}

TEST_CASE("memcached dump") {
  MemcachedCluster tests[]{
      MemcachedCluster::mixed(),
      MemcachedCluster::net(),
      MemcachedCluster::socket()
  };

  LOOPED_SECTION(tests) {
    auto memc = &test.memc;

    SECTION("prepared with 64 KVs") {
      for (int i = 0; i < 64; ++i) {
        char key[8];
        int len = snprintf(key, sizeof(key) - 1, "k_%d", i);

        CHECKED_IF(len) {
          auto rc = memcached_set(memc, key, len, key, len, 0, 0);
          INFO("last error: " << memcached_last_error(memc));
          REQUIRE(MEMCACHED_SUCCESS == rc);
        }
      }

      memcached_quit(memc);

      // let memcached sort itself
      using namespace chrono_literals;
      this_thread::sleep_for(3s);

      SECTION("dumps 64 KVs") {
        size_t counter = 0;
        memcached_dump_fn fn[] = {dump_cb};

        REQUIRE(MEMCACHED_SUCCESS == memcached_dump(memc, fn, &counter, 1));
        REQUIRE(counter == 64);
      }
    }
  }
}
