#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_lp421108") {
  MemcachedCluster test;
  auto memc = &test.memc;

  memcached_return_t rc;
  auto memc_stat = memcached_stat(memc, NULL, &rc);
  REQUIRE_SUCCESS(rc);

  // stat_get_value used memcmp

  Malloced bytes_str(memcached_stat_get_value(memc, memc_stat, "bytes", &rc));
  REQUIRE_SUCCESS(rc);
  REQUIRE(*bytes_str);

  Malloced bytes_read_str(memcached_stat_get_value(memc, memc_stat,"bytes_read", &rc));
  REQUIRE_SUCCESS(rc);
  REQUIRE(*bytes_read_str);

  Malloced bytes_written_str(memcached_stat_get_value(memc, memc_stat, "bytes_written", &rc));
  REQUIRE_SUCCESS(rc);
  REQUIRE(*bytes_written_str);

  REQUIRE(strcmp(*bytes_str, *bytes_read_str));
  REQUIRE(strcmp(*bytes_str, *bytes_written_str));
  REQUIRE(strcmp(*bytes_read_str, *bytes_written_str));

  memcached_stat_free(nullptr, memc_stat);
}
