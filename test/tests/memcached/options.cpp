#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

#include "libmemcached/options.hpp"
#include "libmemcached/common.h"
#include "libmemcached/array.h"


#include <libgen.h>

#define FIXTURES_OPTIONS_CONFIG "/../../fixtures/options.config"

TEST_CASE("memcached_options") {
  MemcachedPtr memc;
  LoneReturnMatcher test{*memc};

  char cpp_file[] = __FILE__, buf[1024], *dir = dirname(cpp_file);
  auto file = string(dir) + FIXTURES_OPTIONS_CONFIG;
  auto config = "--CONFIGURE-FILE=\"" + file + "\"";

  REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS, memcached_parse_configure_file(**memc, nullptr, 123));
  REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS, memcached_parse_configure_file(**memc, file.c_str(), 0));
  REQUIRE_RC(MEMCACHED_ERRNO, memcached_parse_configure_file(**memc, S("foobar.baz")));
  REQUIRE_SUCCESS(memcached_parse_configure_file(**memc, file.c_str(), file.length()));

  REQUIRE_SUCCESS(libmemcached_check_configuration(config.c_str(), config.length(), buf, sizeof(buf) - 1));
  REQUIRE_RC(MEMCACHED_PARSE_ERROR, libmemcached_check_configuration(S("foo bar"), nullptr, 0));
}
