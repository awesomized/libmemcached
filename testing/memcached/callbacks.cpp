#include "../lib/common.hpp"

static memcached_return_t delete_trigger(memcached_st *, const char *, size_t) {
  return MEMCACHED_SUCCESS;
}

TEST_CASE("memcached callbacks") {
  void *fptr = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(&delete_trigger));
  MemcachedPtr memc;

  SECTION("set delete trigger") {
    REQUIRE(MEMCACHED_SUCCESS == memcached_callback_set(*memc, MEMCACHED_CALLBACK_DELETE_TRIGGER, fptr));
  }

  SECTION("set delete trigger fails w/ NOREPLY") {
    REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set(*memc, MEMCACHED_BEHAVIOR_NOREPLY, true));
    REQUIRE_FALSE(MEMCACHED_SUCCESS == memcached_callback_set(*memc, MEMCACHED_CALLBACK_DELETE_TRIGGER, fptr));
  }
}
