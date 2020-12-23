#include "test/lib/common.hpp"

#include "libmemcachedutil-1.0/pool.h"
#include <cassert>

struct test_pool_context_st {
  volatile memcached_return_t rc;
  memcached_pool_st *pool;
  memcached_st *memc;

  test_pool_context_st(memcached_pool_st *pool_arg, memcached_st *memc_arg)
  : rc(MEMCACHED_FAILURE)
  , pool(pool_arg)
  , memc(memc_arg)
  {
  }
};

static void *connection_release(void *arg) {
  assert(arg != nullptr);

  this_thread::sleep_for(200ms);
  auto res = static_cast<test_pool_context_st *>(arg);
  res->rc = memcached_pool_release(res->pool, res->memc);

  pthread_exit(arg);
}

TEST_CASE("memcached_util_pool_thread") {
  MemcachedPtr memc;
  auto pool = memcached_pool_create(*memc, 1, 1);
  REQUIRE(pool);

  memcached_return_t rc;
  auto pool_memc = memcached_pool_fetch(pool, nullptr, &rc);
  REQUIRE(MEMCACHED_SUCCESS == rc);
  REQUIRE(pool_memc);

  /*
    @note This comment was written to describe what was believed to be the original authors intent.

    This portion of the test creates a thread that will wait until told to free a memcached_st
    that will be grabbed by the main thread.

    It is believed that this tests whether or not we are handling ownership correctly.
  */
  pthread_t tid;
  test_pool_context_st item(pool, pool_memc);

  REQUIRE(0 == pthread_create(&tid, nullptr, connection_release, &item));

  memcached_st *pop_memc;
  do {
    struct timespec relative_time = {0, 0};
    pop_memc = memcached_pool_fetch(pool, &relative_time, &rc);

    if (memcached_success(rc)) {
      break;
    }

    if (memcached_failed(rc)) {
      REQUIRE_FALSE(pop_memc);
      REQUIRE(rc != MEMCACHED_TIMEOUT); // As long as relative_time is zero, MEMCACHED_TIMEOUT is invalid
    }
  } while (rc == MEMCACHED_NOTFOUND);

  // Cleanup thread since we will exit once we test.
  REQUIRE(0 == pthread_join(tid, nullptr));
  REQUIRE(MEMCACHED_SUCCESS == rc);
  REQUIRE(pool_memc == pop_memc);
  REQUIRE(MEMCACHED_SUCCESS == memcached_pool_release(pool, pop_memc));
  REQUIRE(memcached_pool_destroy(pool) == *memc);

}
