#include "../lib/common.hpp"

#include "libmemcached/is.h"

TEST_CASE("memcached basic") {
  memcached_st memc, *memc_ptr;

  memc_ptr = memcached_create(&memc);

  REQUIRE(memc_ptr);
  REQUIRE(memc_ptr == &memc);

  SECTION("can be cloned") {
    memc_ptr = memcached_clone(nullptr, &memc);
    REQUIRE(memc_ptr);
    REQUIRE(memcached_is_allocated(memc_ptr));
    memcached_free(memc_ptr);
  }

  SECTION("can be reset") {
    memc_ptr = memcached_clone(nullptr, &memc);
    REQUIRE(MEMCACHED_SUCCESS == memcached_reset(&memc));
    REQUIRE_FALSE(memcached_is_allocated(&memc));
    REQUIRE(MEMCACHED_SUCCESS == memcached_reset(memc_ptr));
    REQUIRE(memcached_is_allocated(memc_ptr));
    memcached_free(memc_ptr);
  }
}
