#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

#include "test/fixtures/ketama.hpp"

#include "libmemcached/continuum.hpp"
#include "libmemcached/instance.hpp"

TEST_CASE("memcached_ketama_compat") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED, 1));
  REQUIRE(uint64_t(1) == memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_KETAMA_WEIGHTED));

  SECTION("generate hash") {
    REQUIRE_SUCCESS(memcached_behavior_set_distribution(memc, MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA));
    REQUIRE(MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA == memcached_behavior_get_distribution(memc));

    memcached_server_st *server_pool = memcached_servers_parse(
        "10.0.1.1:11211 600,10.0.1.2:11211 300,10.0.1.3:11211 200,10.0.1.4:11211 350,10.0.1.5:11211 1000,10.0.1.6:11211 800,10.0.1.7:11211 950,10.0.1.8:11211 100"
    );
    memcached_servers_reset(memc);
    memcached_server_push(memc, server_pool);

    /* verify that the server list was parsed okay. */
    REQUIRE(8U == memcached_server_count(memc));
    REQUIRE(server_pool[0].hostname == "10.0.1.1"s);
    REQUIRE(in_port_t(11211) == server_pool[0].port);
    REQUIRE(600U == server_pool[0].weight);
    REQUIRE(server_pool[2].hostname == "10.0.1.3"s);
    REQUIRE(in_port_t(11211) == server_pool[2].port);
    REQUIRE(200U == server_pool[2].weight);
    REQUIRE(server_pool[7].hostname == "10.0.1.8"s);
    REQUIRE(in_port_t(11211) == server_pool[7].port);
    REQUIRE(100U == server_pool[7].weight);

    /* VDEAAAAA hashes to fffcd1b5, after the last continuum point, and lets
     * us test the boundary wraparound.
     */
    REQUIRE(memcached_generate_hash(memc, (char *) "VDEAAAAA", 8) == memc->ketama.continuum[0].index);

    /* verify the standard ketama set. */
    for (uint32_t x = 0; x < 99; x++) {
      uint32_t server_idx = memcached_generate_hash(memc, ketama_test_cases[x].key, strlen(ketama_test_cases[x].key));
      const memcached_instance_st *instance =
          memcached_server_instance_by_position(memc, server_idx);
      const char *hostname = memcached_server_name(instance);

      REQUIRE(string{hostname} == ketama_test_cases[x].server);
    }

    memcached_server_list_free(server_pool);
  }

  SECTION("user bug 18") {

    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_KETAMA_HASH, MEMCACHED_HASH_MD5));
    REQUIRE(MEMCACHED_HASH_MD5 == memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_KETAMA_HASH));

    memcached_server_st *server_pool = memcached_servers_parse(
        "10.0.1.1:11211 600,10.0.1.2:11211 300,10.0.1.3:11211 200,10.0.1.4:11211 350,10.0.1.5:11211 1000,10.0.1.6:11211 800,10.0.1.7:11211 950,10.0.1.8:11211 100"
    );
    memcached_servers_reset(memc);
    memcached_server_push(memc, server_pool);

    /* verify that the server list was parsed okay. */
    REQUIRE(memcached_server_count(memc) == 8);
    REQUIRE(server_pool[0].hostname == "10.0.1.1"s);
    REQUIRE(server_pool[0].port == 11211);
    REQUIRE(server_pool[0].weight == 600);
    REQUIRE(server_pool[2].hostname == "10.0.1.3"s);
    REQUIRE(server_pool[2].port == 11211);
    REQUIRE(server_pool[2].weight == 200);
    REQUIRE(server_pool[7].hostname == "10.0.1.8"s);
    REQUIRE(server_pool[7].port == 11211);
    REQUIRE(server_pool[7].weight == 100);

    /* VDEAAAAA hashes to fffcd1b5, after the last continuum point, and lets
     * us test the boundary wraparound.
     */
    REQUIRE(memcached_generate_hash(memc, (char *) "VDEAAAAA", 8) == memc->ketama.continuum[0].index);

    /* verify the standard ketama set. */
    for (auto x = 0; x < 99; x++) {
      uint32_t server_idx = memcached_generate_hash(memc, ketama_test_cases[x].key, strlen(ketama_test_cases[x].key));

      const memcached_instance_st *instance =
          memcached_server_instance_by_position(memc, server_idx);

      const char *hostname = memcached_server_name(instance);
      REQUIRE(string{hostname} == ketama_test_cases[x].server);
    }

    memcached_server_list_free(server_pool);
  }

  SECTION("auto_eject_hosts") {
    memcached_servers_reset(memc);

    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_KETAMA_HASH, MEMCACHED_HASH_MD5));
    REQUIRE(MEMCACHED_HASH_MD5 == memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_KETAMA_HASH));

    /* server should be removed when in delay */
    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, 1));
    REQUIRE(memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS));

    memcached_server_st *server_pool;
    server_pool = memcached_servers_parse(
        "10.0.1.1:11211 600,10.0.1.2:11211 300,10.0.1.3:11211 200,10.0.1.4:11211 350,10.0.1.5:11211 1000,10.0.1.6:11211 800,10.0.1.7:11211 950,10.0.1.8:11211 100"
    );
    memcached_servers_reset(memc);
    memcached_server_push(memc, server_pool);

    /* verify that the server list was parsed okay. */
    REQUIRE(memcached_server_count(memc) == 8);
    REQUIRE(server_pool[0].hostname == "10.0.1.1"s);
    REQUIRE(server_pool[0].port == 11211);
    REQUIRE(server_pool[0].weight == 600);
    REQUIRE(server_pool[2].hostname == "10.0.1.3"s);
    REQUIRE(server_pool[2].port == 11211);
    REQUIRE(server_pool[2].weight == 200);
    REQUIRE(server_pool[7].hostname == "10.0.1.8"s);
    REQUIRE(server_pool[7].port == 11211);
    REQUIRE(server_pool[7].weight == 100);

    const memcached_instance_st *instance = memcached_server_instance_by_position(memc, 2);
    memcached_instance_next_retry(instance, time(nullptr) + 15);
    memc->ketama.next_distribution_rebuild = time(nullptr) - 1;

    /*
      This would not work if there were only two hosts.
    */
    for (auto x = 0; x < 99; x++) {
      memcached_autoeject(memc);
      uint32_t server_idx = memcached_generate_hash(memc, ketama_test_cases[x].key, strlen(ketama_test_cases[x].key));
      REQUIRE(server_idx != 2);
    }

    /* and re-added when it's back. */
    time_t absolute_time = time(nullptr) - 1;
    memcached_instance_next_retry(instance, absolute_time);
    memc->ketama.next_distribution_rebuild = absolute_time;
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_DISTRIBUTION, memc->distribution);
    for (auto x = 0; x < 99; x++) {
      uint32_t server_idx = memcached_generate_hash(memc, ketama_test_cases[x].key, strlen(ketama_test_cases[x].key));
      // We re-use instance from above.
      instance = memcached_server_instance_by_position(memc, server_idx);
      const char *hostname = memcached_server_name(instance);
      REQUIRE(string{hostname} == ketama_test_cases[x].server);
    }

    memcached_server_list_free(server_pool);
  }

  SECTION("spymemcached") {
    memcached_servers_reset(memc);
    REQUIRE_SUCCESS(memcached_behavior_set_distribution(memc, MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA_SPY));
    REQUIRE(MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA_SPY == memcached_behavior_get_distribution(memc));

    memcached_server_st *server_pool = memcached_servers_parse(
        "10.0.1.1:11211 600,10.0.1.2:11211 300,10.0.1.3:11211 200,10.0.1.4:11211 350,10.0.1.5:11211 1000,10.0.1.6:11211 800,10.0.1.7:11211 950,10.0.1.8:11211 100"
    );
    REQUIRE(server_pool);
    memcached_server_push(memc, server_pool);

    /* verify that the server list was parsed okay. */
    REQUIRE(8U == memcached_server_count(memc));
    REQUIRE(server_pool[0].hostname == "10.0.1.1"s);
    REQUIRE(in_port_t(11211) == server_pool[0].port);
    REQUIRE(600U == server_pool[0].weight);
    REQUIRE(server_pool[2].hostname == "10.0.1.3"s);
    REQUIRE(in_port_t(11211) == server_pool[2].port);
    REQUIRE(200U == server_pool[2].weight);
    REQUIRE(server_pool[7].hostname == "10.0.1.8"s);
    REQUIRE(in_port_t(11211) == server_pool[7].port);
    REQUIRE(100U == server_pool[7].weight);

    /* VDEAAAAA hashes to fffcd1b5, after the last continuum point, and lets
     * us test the boundary wraparound.
     */
    REQUIRE(memcached_generate_hash(memc, (char *) "VDEAAAAA", 8) == memc->ketama.continuum[0].index);

    /* verify the standard ketama set. */
    for (uint32_t x = 0; x < 99; x++) {
      uint32_t server_idx = memcached_generate_hash(memc, ketama_test_cases_spy[x].key,
          strlen(ketama_test_cases_spy[x].key));

      const memcached_instance_st *instance =
          memcached_server_instance_by_position(memc, server_idx);

      const char *hostname = memcached_server_name(instance);
      REQUIRE(string{hostname} == ketama_test_cases_spy[x].server);
    }

    memcached_server_list_free(server_pool);
  }
}
