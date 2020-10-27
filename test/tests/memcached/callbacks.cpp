#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"

TEST_CASE("memcached_callbacks") {
  auto test{MemcachedCluster::mixed()};
  auto memc = &test.memc;
  memcached_return_t rc;

  SECTION("user data") {
    int temp = 123;
    void *data = &temp;

    REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_USER_DATA, data));
    REQUIRE(data == memcached_callback_get(memc, MEMCACHED_CALLBACK_USER_DATA, &rc));
    REQUIRE_SUCCESS(rc);
  }

  SECTION("delete callback") {
    void *dptr = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(&delete_trigger));

    REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_DELETE_TRIGGER, dptr));
    REQUIRE(memcached_callback_get(memc, MEMCACHED_CALLBACK_DELETE_TRIGGER, &rc) == dptr);
    REQUIRE_SUCCESS(rc);

    SECTION("fails w/ NOREPLY") {
      REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NOREPLY, true));
      REQUIRE_RC(MEMCACHED_INVALID_ARGUMENTS, memcached_callback_set(memc, MEMCACHED_CALLBACK_DELETE_TRIGGER, dptr));
    }
  }

  SECTION("get_failure callback") {
    void *gptr = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(&get_failure));
    Malloced empty(memcached_get(memc, S(__func__), nullptr, nullptr, &rc));
    REQUIRE_FALSE(*empty);
    REQUIRE_RC(MEMCACHED_NOTFOUND, rc);

    uint64_t buffering = GENERATE(0, 1);

    DYNAMIC_SECTION("buffering: " << buffering) {
      REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, buffering));
      REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_GET_FAILURE, gptr));
      REQUIRE(gptr == memcached_callback_get(memc, MEMCACHED_CALLBACK_GET_FAILURE, &rc));
      REQUIRE_SUCCESS(rc);

      for (int twice = 0; twice < 2; ++twice) {
        uint32_t flags;
        size_t len;
        Malloced val(memcached_get(memc, S(__func__), &len, &flags, &rc));

        REQUIRE_SUCCESS(rc);
        REQUIRE(string("updated by read through trigger") == string(*val, len));
        REQUIRE_FALSE((*val)[len]);
      }

      REQUIRE_SUCCESS(memcached_set(memc, S(__func__), S("changed"), 0, 0));
      memcached_quit(memc);
      Malloced val(memcached_get(memc, S(__func__), nullptr, nullptr, &rc));
      REQUIRE_SUCCESS(rc);
      REQUIRE("changed"s == *val);
    }
  }

  SECTION("clone callback") {
    void *cptr = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(&clone_callback));

    REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_CLONE_FUNCTION, cptr));
    REQUIRE(cptr == memcached_callback_get(memc, MEMCACHED_CALLBACK_CLONE_FUNCTION, &rc));
    REQUIRE_SUCCESS(rc);
  }

  SECTION("cleanup callback") {
    void *cptr = reinterpret_cast<void *>(reinterpret_cast<intptr_t>(cleanup_callback));

    REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_CLEANUP_FUNCTION, cptr));
    REQUIRE(cptr == memcached_callback_get(memc, MEMCACHED_CALLBACK_CLEANUP_FUNCTION, &rc));
    REQUIRE_SUCCESS(rc);
  }

  SECTION("namespace") {
    void *ns;

    REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_NAMESPACE, "ns"));
    ns = memcached_callback_get(memc, MEMCACHED_CALLBACK_NAMESPACE, &rc);
    REQUIRE_SUCCESS(rc);
    REQUIRE("ns"s == static_cast<char *>(ns));

    REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_NAMESPACE, nullptr));
    ns = memcached_callback_get(memc, MEMCACHED_CALLBACK_NAMESPACE, &rc);
    REQUIRE_SUCCESS(rc);
    REQUIRE(nullptr == ns);


    uint64_t binary = GENERATE(0, 1);
    REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, binary));

    DYNAMIC_SECTION("too long (binary=" << binary << ")") {
      string blob;

      blob = random_ascii_string(MEMCACHED_MAX_NAMESPACE-1, '@', 'Z');
      REQUIRE_SUCCESS(memcached_callback_set(memc, MEMCACHED_CALLBACK_NAMESPACE, blob.c_str()));

      blob = random_ascii_string(MEMCACHED_MAX_NAMESPACE, '@', 'Z');
      REQUIRE_RC(MEMCACHED_KEY_TOO_BIG,memcached_callback_set(memc, MEMCACHED_CALLBACK_NAMESPACE, blob.c_str()));
    }

    DYNAMIC_SECTION("verify key (binary=" << binary << ")") {
      REQUIRE_RC(binary ? MEMCACHED_INVALID_ARGUMENTS : MEMCACHED_SUCCESS,
          memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_VERIFY_KEY, 1));
      REQUIRE_RC(binary ? MEMCACHED_SUCCESS : MEMCACHED_BAD_KEY_PROVIDED,
          memcached_callback_set(memc, MEMCACHED_CALLBACK_NAMESPACE, "with spaces"));
    }
  }
}
