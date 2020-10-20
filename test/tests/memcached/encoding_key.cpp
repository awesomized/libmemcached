#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"

#define TEST_KEY S("test")
#define INITIAL_VAL S("initial value, which is longer than AES_BLOCK_SIZE")
#define REPLACED_VAL S("replaced value, thich is longer than AES_BLOCK_SIZE")

static inline void check(memcached_st *enc, memcached_st *raw, const char *val, size_t len) {
  memcached_return_t enc_rc, raw_rc;
  size_t enc_length, raw_length;
  Malloced enc_value(memcached_get(enc, TEST_KEY, &enc_length, nullptr, &enc_rc));
  Malloced raw_value(memcached_get(raw, TEST_KEY, &raw_length, nullptr, &raw_rc));

  REQUIRE(enc_rc == MEMCACHED_SUCCESS);
  REQUIRE(raw_rc == MEMCACHED_SUCCESS);
  REQUIRE_FALSE(enc_length == raw_length);
  REQUIRE(memcmp(*raw_value, *enc_value, raw_length));
  REQUIRE(enc_length == len);
  REQUIRE_FALSE(memcmp(val, *enc_value, enc_length));
}

TEST_CASE("memcached_encoding_key") {
  pair<string, MemcachedCluster> tests[]{
    {"network", MemcachedCluster::network()},
    {"socket", MemcachedCluster::socket()}
  };

  LOOPED_SECTION(tests) {
    auto memc = &test.memc;

    SECTION("accepts encoding key") {
      MemcachedPtr copy(memcached_clone(nullptr, memc));

      REQUIRE_SUCCESS(memcached_set_encoding_key(memc, S(__func__)));

      SECTION("sets encoded value") {
        REQUIRE_SUCCESS(memcached_set(memc, TEST_KEY, INITIAL_VAL, 0, 0));

        SECTION("gets encoded value") {
          check(memc, copy.memc, INITIAL_VAL);
        }

        SECTION("cloned gets encoded value") {
          MemcachedPtr dupe(memcached_clone(nullptr, memc));

          check(dupe.memc, copy.memc, INITIAL_VAL);
        }
      }

      SECTION("adds encoded value") {

        REQUIRE_SUCCESS(memcached_set(memc, TEST_KEY, INITIAL_VAL, 0, 0));
        REQUIRE_RC(MEMCACHED_NOTSTORED, memcached_add(memc, TEST_KEY, REPLACED_VAL, 0, 0));

        check(memc, copy.memc, INITIAL_VAL);

        test.flush();

        REQUIRE_SUCCESS(memcached_add(memc, TEST_KEY, REPLACED_VAL, 0, 0));

        SECTION("gets encoded value") {
          check(memc, copy.memc, REPLACED_VAL);
        }
      }

      SECTION("replaces encoded value") {
        REQUIRE_SUCCESS(memcached_set(memc, TEST_KEY, INITIAL_VAL, 0, 0));

        check(memc, copy.memc, INITIAL_VAL);

        REQUIRE_SUCCESS(memcached_replace(memc, TEST_KEY, REPLACED_VAL, 0, 0));

        SECTION("gets encoded value") {
          check(memc, copy.memc, REPLACED_VAL);
        }
      }

      SECTION("unsupported") {
        REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_increment(memc, TEST_KEY, 0, nullptr));
        REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_decrement(memc, TEST_KEY, 0, nullptr));
        REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_increment_with_initial(memc, TEST_KEY, 0, 0, 0, nullptr));
        REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_decrement_with_initial(memc, TEST_KEY, 0, 0, 0, nullptr));
        REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_append(memc, TEST_KEY, REPLACED_VAL, 0, 0));
        REQUIRE_RC(MEMCACHED_NOT_SUPPORTED, memcached_prepend(memc, TEST_KEY, REPLACED_VAL, 0, 0));
      }
    }
  }
}
