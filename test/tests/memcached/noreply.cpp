#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

enum action_t {ADD, REPLACE, SET, APPEND, PREPEND};
inline action_t operator ++ (action_t &a) {
  return a = static_cast<action_t>(underlying_type<action_t>::type(a) + 1);
}

constexpr static const int keys = 5000;

TEST_CASE("memcached_noreply") {
  auto test{MemcachedCluster::mixed()};
  auto memc = &test.memc;

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NOREPLY, true));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, true));
  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SUPPORT_CAS, true));

  for (auto action = ADD; action <= PREPEND; ++action) {
    for (auto i = 0; i < keys; ++i) {
      auto key = to_string(i);
      memcached_return_t rc;

      switch (action) {
      case ADD:     rc = memcached_add(memc, key.c_str(), key.length(), key.c_str(), key.length(), 0, 0); break;
      case REPLACE: rc = memcached_replace(memc, key.c_str(), key.length(), key.c_str(), key.length(), 0, 0); break;
      case SET:     rc = memcached_set(memc, key.c_str(), key.length(), key.c_str(), key.length(), 0, 0); break;
      case APPEND:  rc = memcached_append(memc, key.c_str(), key.length(), key.c_str(), key.length(), 0, 0); break;
      case PREPEND: rc = memcached_prepend(memc, key.c_str(), key.length(), key.c_str(), key.length(), 0, 0); break;
      default:      FAIL();
      }

      if (rc != MEMCACHED_BUFFERED) {
        REQUIRE_SUCCESS(rc);
      }
    }

    REQUIRE_SUCCESS(memcached_flush_buffers(memc));

    for (auto i = 0; i < keys; ++i) {
      auto key = to_string(i);

      size_t len;
      memcached_return_t rc;
      uint32_t flags;
      Malloced val(memcached_get(memc, key.c_str(), key.length(), &len, &flags, &rc));

      REQUIRE_SUCCESS(rc);
      REQUIRE(*val);

      switch (action) {
      case ADD:     [[fallthrough]];
      case REPLACE: [[fallthrough]];
      case SET:     REQUIRE(key == *val);  break;
      case APPEND:  REQUIRE(key + key == *val); break;
      case PREPEND: REQUIRE(key + key + key == *val); break;
      default:      FAIL();
      }
    }
  }
}
