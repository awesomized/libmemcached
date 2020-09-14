#include "testing/lib/common.hpp"
#include "testing/lib/MemcachedCluster.hpp"

#include "libmemcached/instance.hpp"
#include "libmemcachedutil/common.h"

static memcached_return_t ping_callback(const memcached_st *, const memcached_instance_st *instance, void *) {
  memcached_return_t rc;

  REQUIRE(libmemcached_util_ping(memcached_server_name(instance), memcached_server_port(instance), &rc));
  REQUIRE(rc == MEMCACHED_SUCCESS);
  return MEMCACHED_SUCCESS;
}

TEST_CASE("memcached_util") {
  SECTION("version_check") {
    auto test = MemcachedCluster::mixed();
    auto memc = &test.memc;

    REQUIRE_SUCCESS(memcached_version(memc));
    REQUIRE(libmemcached_util_version_check(memc, 0, 0, 0));
    REQUIRE_FALSE(libmemcached_util_version_check(memc, 255, 255, 255));

    auto instance = memcached_server_instance_by_position(memc, 0);
    REQUIRE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version, instance->micro_version));
    if (instance->micro_version) {
      REQUIRE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version, instance->micro_version - 1));
      if (instance->micro_version < 255) {
        REQUIRE_FALSE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version, instance->micro_version + 1));
      }
    }
    if (instance->minor_version) {
      REQUIRE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version - 1, 255));
      if (instance->minor_version < 255) {
        REQUIRE_FALSE(libmemcached_util_version_check(memc, instance->major_version, instance->minor_version + 1, 0));
      }
    }
    if (instance->major_version) {
      REQUIRE(libmemcached_util_version_check(memc, instance->major_version - 1, 255, 255));
      if (instance->major_version < 255) {
        REQUIRE_FALSE(libmemcached_util_version_check(memc, instance->major_version + 1, 0, 0));
      }
    }
  }

  SECTION("getpid") {
    auto test = MemcachedCluster::network();
    memcached_return_t rc;

    for (auto &server : test.cluster.getServers()) {
      REQUIRE(server.getPid() == libmemcached_util_getpid("localhost", get<int>(server.getSocketOrPort()), &rc));
      REQUIRE_SUCCESS(rc);
    }

    REQUIRE(-1 == libmemcached_util_getpid("localhost", 1, &rc));
    REQUIRE_RC(MEMCACHED_CONNECTION_FAILURE, rc);
  }

  SECTION("ping") {
    auto test = MemcachedCluster::network();
    auto memc = &test.memc;
    memcached_server_fn fptr[] = {&ping_callback};

    REQUIRE_SUCCESS(memcached_server_cursor(memc, fptr, nullptr, 1));
  }

  SECTION("flush") {
    auto test = MemcachedCluster::network();

    for (auto &server : test.cluster.getServers()) {
      memcached_return_t rc;
      REQUIRE(libmemcached_util_flush("localhost", get<int>(server.getSocketOrPort()), &rc));
      REQUIRE_SUCCESS(rc);
    }
  }
}
