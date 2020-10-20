#include "test/lib/common.hpp"
#include "test/lib/MemcachedCluster.hpp"
#include "test/fixtures/callbacks.hpp"
#include "libmemcached/instance.hpp"

#define NUM_KEYS 100U

TEST_CASE("memcached_regression_lp447342") {
  auto test = MemcachedCluster::network();
  auto memc = &test.memc;

  REQUIRE_SUCCESS(memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, 2));

  array<string, NUM_KEYS> str;
  array<char *, NUM_KEYS> chr;
  array<size_t, NUM_KEYS> len;

  for (auto i = 0U; i < NUM_KEYS; ++i) {
    str[i] = random_ascii_string(random_num(12, 16)) + to_string(i);
    chr[i] = str[i].data();
    len[i] = str[i].length();
    REQUIRE_SUCCESS(memcached_set(memc, chr[i], len[i], chr[i], len[i], 0, 0));
  }

 /*
 ** We are using the quit command to store the replicas, so we need
 ** to ensure that all of them are processed before we can continue.
 ** In the test we go directly from storing the object to trying to
 ** receive the object from all of the different servers, so we
 ** could end up in a race condition (the memcached server hasn't yet
 ** processed the quiet command from the replication set when it process
 ** the request from the other client (created by the clone)). As a
 ** workaround for that we call memcached_quit to send the quit command
 ** to the server and wait for the response ;-) If you use the test code
 ** as an example for your own code, please note that you shouldn't need
 ** to do this ;-)
 */

 memcached_quit(memc);

 REQUIRE_SUCCESS(memcached_mget(memc, chr.data(), len.data(), NUM_KEYS));

 size_t counter = 0;
 memcached_execute_fn cb[] = {&callback_counter};
 REQUIRE_SUCCESS(memcached_fetch_execute(memc, cb, &counter, 1));
 REQUIRE(counter == NUM_KEYS);

 memcached_quit(memc);

  /*
  * Don't do the following in your code. I am abusing the internal details
  * within the library, and this is not a supported interface.
  * This is to verify correct behavior in the library. Fake that two servers
  * are dead..
  */
  auto instance_one= memcached_server_instance_by_position(memc, 0);
  auto instance_two= memcached_server_instance_by_position(memc, 2);
  in_port_t port0= instance_one->port();
  in_port_t port2= instance_two->port();

  const_cast<memcached_instance_st*>(instance_one)->port(0);
  const_cast<memcached_instance_st*>(instance_two)->port(0);

  REQUIRE_SUCCESS(memcached_mget(memc, chr.data(), len.data(), NUM_KEYS));

  counter = 0;
  REQUIRE_SUCCESS(memcached_fetch_execute(memc, cb, &counter, 1));
  REQUIRE(counter == NUM_KEYS);

  /* restore the memc handle */
  const_cast<memcached_instance_st*>(instance_one)->port(port0);
  const_cast<memcached_instance_st*>(instance_two)->port(port2);

  memcached_quit(memc);

  for (auto i = 0U; i < NUM_KEYS; ++i) {
    if (i & 1U) {
      REQUIRE_SUCCESS(memcached_delete(memc, chr[i], len[i], 0));
    }
  }

  memcached_quit(memc);

  const_cast<memcached_instance_st*>(instance_one)->port(0);
  const_cast<memcached_instance_st*>(instance_two)->port(0);

  /* now retry the command, this time we should have cache misses */
  REQUIRE_SUCCESS(memcached_mget(memc, chr.data(), len.data(), NUM_KEYS));

  counter = 0;
  REQUIRE_SUCCESS(memcached_fetch_execute(memc, cb, &counter, 1));
  REQUIRE(counter == NUM_KEYS>>1U);
}
