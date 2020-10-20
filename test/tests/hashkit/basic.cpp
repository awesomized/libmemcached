#include "test/lib/common.hpp"
#include "test/fixtures/hashes.hpp"

#include "libhashkit-1.0/hashkit.hpp"

TEST_CASE("hashkit") {
  hashkit_st st, *hp = hashkit_create(nullptr);
  Hashkit stack;
  Hashkit *heap = new Hashkit;
  auto newed = unique_ptr<Hashkit>(heap);

  REQUIRE(hashkit_create(&st));
  REQUIRE(hp);

  SECTION("can copy") {
    Hashkit stack_copy(stack);
    Hashkit *heap_copy(heap);
    hashkit_st st_copy, *st_ptr;

    (void) stack_copy;
    (void) heap_copy;

    st_ptr = hashkit_clone(&st_copy, &st);
    REQUIRE(st_ptr == &st_copy);
    REQUIRE(hashkit_compare(st_ptr, &st_copy));

    SUCCEED("OK");
  }

  SECTION("can assign") {
    Hashkit stack_copy;

    stack_copy = stack;
    (void) stack_copy;

    SUCCEED("OK");
  }

  SECTION("can digest default") {
    REQUIRE(2297466611U == stack.digest(S("apple")));
    REQUIRE(2297466611U == hashkit_digest(&st, S("apple")));
  }

  SECTION("can set hash function") {
    for (int f = HASHKIT_HASH_DEFAULT; f < HASHKIT_HASH_MAX; ++f) {
      auto h = static_cast<hashkit_hash_algorithm_t>(f);

      if (h == HASHKIT_HASH_CUSTOM) {
        continue;
      }
      if (!libhashkit_has_algorithm(h)) {
        WARN("hashkit algorithm not enabled: " << libhashkit_string_hash(h) << " (" << f << ")");
        continue;
      }

      INFO("hash: " << libhashkit_string_hash(h));

      REQUIRE(HASHKIT_SUCCESS == stack.set_function(h));
      REQUIRE(HASHKIT_SUCCESS == hashkit_set_function(&st, h));

      auto n = 0;
      for (auto i : input) {
        CHECK(output[f][n] == stack.digest(S(i)));
        CHECK(output[f][n] == hashkit_digest(&st, S(i)));
        CHECK(output[f][n] == libhashkit_digest(S(i), h));
        ++n;
      }
    }
  }

  SECTION("is comparable") {
    REQUIRE(*heap == stack);
    REQUIRE(hashkit_compare(&st, hp));

    stack.set_function(HASHKIT_HASH_MD5);
    hashkit_set_function(&st, HASHKIT_HASH_MD5);

    REQUIRE_FALSE(*heap == stack);
    REQUIRE_FALSE(hashkit_compare(&st, hp));
  }

  SECTION("strerror") {
    auto bad_str = string{hashkit_strerror(hp, HASHKIT_MAXIMUM_RETURN)};
    for (int h = HASHKIT_SUCCESS; h < HASHKIT_MAXIMUM_RETURN; ++h) {
      auto r = static_cast<hashkit_return_t>(h);
      REQUIRE(bad_str != hashkit_strerror(hp, r));
    }
  }

  hashkit_free(&st);
  hashkit_free(hp);
}
