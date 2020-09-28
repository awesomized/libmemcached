#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

TEST_CASE("memcached_inc_dec") {
  pair<string, MemcachedCluster> tests[] = {
      {"mixed", MemcachedCluster::mixed()},
  };

  LOOPED_SECTION(tests) {
    auto memc = &test.memc;
    uint64_t binary = GENERATE(0, 1);
    char *prefix = GENERATE(as<char *>{}, "", "namespace:");

    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, binary));
    if (*prefix) {
      REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_NAMESPACE, prefix));
    }

    DYNAMIC_SECTION("increment (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;

      REQUIRE_SUCCESS(memcached_set(memc, S("number"), S("0"), 0, 0));

      for (auto i = 1; i <= 10; ++i) {
        REQUIRE_SUCCESS(memcached_increment(memc, S("number"), 1, &number));
        REQUIRE(number == static_cast<uint64_t>(i));
      }
    }

    DYNAMIC_SECTION("increment by_key (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;

      REQUIRE_SUCCESS(memcached_set_by_key(memc, S("key"), S("number"), S("0"), 0, 0));

      for (auto i = 1; i <= 10; ++i) {
        REQUIRE_SUCCESS(memcached_increment_by_key(memc, S("key"), S("number"), 1, &number));
        REQUIRE(number == static_cast<uint64_t>(i));
      }
    }

    DYNAMIC_SECTION("increment with initial (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;
      uint64_t initial = GENERATE(0, 456);

      if (!binary) {
        REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS,
            memcached_increment_with_initial(memc, S("number"), 1, initial, 0, &number));
      } else {
        REQUIRE_SUCCESS(memcached_increment_with_initial(memc, S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial);
        REQUIRE_SUCCESS(memcached_increment_with_initial(memc, S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial + 123);
      }
    }

    DYNAMIC_SECTION("increment with initial by_key (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;
      uint64_t initial = GENERATE(0, 456);

      if (!binary) {
        REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS,
            memcached_increment_with_initial_by_key(memc, S("key"), S("number"), 1, initial, 0, &number));
      } else {
        REQUIRE_SUCCESS(memcached_increment_with_initial_by_key(memc, S("key"), S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial);
        REQUIRE_SUCCESS(memcached_increment_with_initial_by_key(memc, S("key"), S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial + 123);
      }
    }

    DYNAMIC_SECTION("decrement (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;

      REQUIRE_SUCCESS(memcached_set(memc, S("number"), S("10"), 0, 0));

      for (auto i = 9; i >= 0; --i) {
        REQUIRE_SUCCESS(memcached_decrement(memc, S("number"), 1, &number));
        REQUIRE(number == static_cast<uint64_t>(i));
      }
    }
    DYNAMIC_SECTION("decrement by_key (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;

      REQUIRE_SUCCESS(memcached_set_by_key(memc, S("key"), S("number"), S("10"), 0, 0));

      for (auto i = 9; i >= 0; --i) {
        REQUIRE_SUCCESS(memcached_decrement_by_key(memc, S("key"), S("number"), 1, &number));
        REQUIRE(number == static_cast<uint64_t>(i));
      }
    }
    DYNAMIC_SECTION("decrement with initial (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;
      uint64_t initial = GENERATE(987, 456);

      if (!binary) {
        REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS,
            memcached_decrement_with_initial(memc, S("number"), 1, initial, 0, &number));
      } else {
        REQUIRE_SUCCESS(memcached_decrement_with_initial(memc, S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial);
        REQUIRE_SUCCESS(memcached_decrement_with_initial(memc, S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial - 123);
      }
    }
    DYNAMIC_SECTION("decrement with initial by_key (binary=" << binary << ", prefix=" << prefix << ")") {
      uint64_t number;
      uint64_t initial = GENERATE(987, 456);

      if (!binary) {
        REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS,
            memcached_decrement_with_initial_by_key(memc, S("key"), S("number"), 1, initial, 0, &number));
      } else {
        REQUIRE_SUCCESS(memcached_decrement_with_initial_by_key(memc, S("key"), S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial);
        REQUIRE_SUCCESS(memcached_decrement_with_initial_by_key(memc, S("key"), S("number"), 123, initial, 0, &number));
        REQUIRE(number == initial - 123);
      }
    }
  }
}
