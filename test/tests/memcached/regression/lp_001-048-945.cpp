#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_regression_lp1048945") {
    MemcachedPtr memc_ptr(memcached_create(nullptr));
    auto memc = *memc_ptr;
    LoneReturnMatcher test{memc};
    memcached_return status;

    auto list = memcached_server_list_append_with_weight(nullptr, "a", 11211, 0, &status);
    REQUIRE_SUCCESS(status);

    list = memcached_server_list_append_with_weight(list, "b", 11211, 0, &status);
    REQUIRE_SUCCESS(status);

    list = memcached_server_list_append_with_weight(list, "c", 11211, 0, &status);
    REQUIRE_SUCCESS(status);

    REQUIRE(3 == memcached_server_list_count(list));

    REQUIRE_SUCCESS(memcached_server_push(memc, list));
    REQUIRE_SUCCESS(memcached_server_push(memc, list));
    memcached_server_list_free(list);

    auto server = memcached_server_by_key(memc, S(__func__), &status);
    REQUIRE(server);
    REQUIRE_SUCCESS(status);
}
