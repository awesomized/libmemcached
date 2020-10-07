#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

#include "libmemcachedutil-1.0/pool.h"

#include <atomic>
#include <sstream>

static atomic<bool> running = false;

static inline void set_running(bool is) {
  running.store(is, memory_order_release);
}
static inline bool is_running() {
  return running.load(memory_order_consume);
}

struct worker_ctx {
  memcached_pool_st *pool;

  explicit worker_ctx(memcached_st *memc)
  : pool{memcached_pool_create(memc, 5, 10)}
  {
  }

  ~worker_ctx() {
    memcached_pool_destroy(pool);
  }
};

static void *worker(void *arg) {
  auto *ctx = static_cast<worker_ctx *>(arg);

  while (is_running()) {
    memcached_return_t rc;
    timespec block{0, 1000};
    auto *mc = memcached_pool_fetch(ctx->pool, &block, &rc);

    if (!mc && rc == MEMCACHED_TIMEOUT) {
      continue;
    }
    if (!mc || memcached_failed(rc)) {
      cerr << "failed to fetch connection from pool: "
           << memcached_strerror(nullptr, rc)
           << endl;
      continue;
    }

    auto rs = random_ascii_string(12);
    rc = memcached_set(mc, rs.c_str(), rs.length(), rs.c_str(), rs.length(), 0, 0);
    if (memcached_failed(rc)) {
      cerr << "failed to memcached_set() "
           << memcached_last_error_message(mc)
           << endl;
    }
    rc = memcached_pool_release(ctx->pool, mc);
    if (memcached_failed(rc)) {
      cerr << "failed to release connection to pool: "
           << memcached_strerror(nullptr, rc)
           << endl;
    }
  }

  return ctx;
}

TEST_CASE("memcached_regression_lp962815") {
  auto test = MemcachedCluster::mixed();
  auto memc = &test.memc;

  constexpr auto NUM_THREADS = 20;
  array<pthread_t, NUM_THREADS> tid;
  worker_ctx ctx{memc};

  set_running(true);

  for (auto &t : tid) {
    REQUIRE(0 == pthread_create(&t, nullptr, worker, &ctx));
  }

  this_thread::sleep_for(1s);
  set_running(false);

  for (auto t : tid) {
    void *ret = nullptr;
    REQUIRE(0 == pthread_join(t, &ret));
    REQUIRE(ret == &ctx);
  }
}
