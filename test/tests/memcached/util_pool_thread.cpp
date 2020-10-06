#include "test/lib/common.hpp"

#include "libmemcachedutil-1.0/pool.h"

#include <cassert>

#if HAVE_SEMAPHORE_H

#include "semaphore.h"

#ifndef __APPLE__
struct test_pool_context_st {
  volatile memcached_return_t rc;
  memcached_pool_st *pool;
  memcached_st *memc;
  sem_t _lock;

  test_pool_context_st(memcached_pool_st *pool_arg, memcached_st *memc_arg) :
      rc(MEMCACHED_FAILURE),
      pool(pool_arg),
      memc(memc_arg) {
    sem_init(&_lock, 0, 0);
  }

  void wait() {
    sem_wait(&_lock);
  }

  void release() {
    sem_post(&_lock);
  }

  ~test_pool_context_st() {
    sem_destroy(&_lock);
  }
};

static void *connection_release(void *arg) {
  assert(arg != nullptr);

  this_thread::sleep_for(2s);
  auto res = static_cast<test_pool_context_st *>(arg);
  res->rc = memcached_pool_release(res->pool, res->memc);
  res->release();

  pthread_exit(arg);
}
#endif

TEST_CASE("memcached_util_pool_thread") {
#ifdef __APPLE__
  SUCCEED("skip: pthreads");
#else
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
  item.wait();

  memcached_st *pop_memc;
  // We do a hard loop, and try N times
  int counter = 5;
  do {
    struct timespec relative_time = {0, 0};
    pop_memc = memcached_pool_fetch(pool, &relative_time, &rc);

    if (memcached_success(rc)) {
      break;
    }
cerr << "rc == " << memcached_strerror(nullptr, rc);
    if (memcached_failed(rc)) {
      REQUIRE_FALSE(pop_memc);
      REQUIRE(rc != MEMCACHED_TIMEOUT); // As long as relative_time is zero, MEMCACHED_TIMEOUT is invalid
    }
  } while (--counter);

  // Cleanup thread since we will exit once we test.
  REQUIRE(0 == pthread_join(tid, nullptr));
  REQUIRE(MEMCACHED_SUCCESS == rc);
  REQUIRE(pool_memc == pop_memc);
  REQUIRE(MEMCACHED_SUCCESS == memcached_pool_release(pool, pop_memc));
  REQUIRE(memcached_pool_destroy(pool) == *memc);

#endif __APPLE__
}

#endif // HAVE_SEMAPHORE_H
