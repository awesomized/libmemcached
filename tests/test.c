/*
  Sample test application.
*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fnmatch.h>
#include "server.h"

#include "test.h"

static long int timedif(struct timeval a, struct timeval b)
{
  register int us, s;

  us = (int)(a.tv_usec - b.tv_usec);
  us /= 1000;
  s = (int)(a.tv_sec - b.tv_sec);
  s *= 1000;
  return s + us;
}

static const char *test_strerror(test_return_t code)
{
  switch (code) {
  case TEST_SUCCESS:
    return "ok";
  case TEST_FAILURE:
    return "failed";
  case TEST_MEMORY_ALLOCATION_FAILURE:
    return "memory allocation";
  case TEST_SKIPPED:
    return "skipped";
  case TEST_MAXIMUM_RETURN:
  default:
    fprintf(stderr, "Unknown return value\n");
    abort();
  }

}

int main(int argc, char *argv[])
{
  test_return_t failed;
  unsigned int x;
  char *collection_to_run= NULL;
  char *wildcard= NULL;
  server_startup_st *startup_ptr;
  memcached_server_st *servers;
  world_st world;
  collection_st *collection;
  collection_st *next;
  void *world_ptr;

  memset(&world, 0, sizeof(world_st));
  get_world(&world);
  collection= world.collections;

  if (world.create)
    world_ptr= world.create();
  else
    world_ptr= NULL;

  startup_ptr= (server_startup_st *)world_ptr;
  servers= (memcached_server_st *)startup_ptr->servers;

  if (argc > 1)
    collection_to_run= argv[1];

  if (argc == 3)
    wildcard= argv[2];

  for (next= collection; next->name; next++)
  {
    test_st *run;

    run= next->tests;
    if (collection_to_run && fnmatch(collection_to_run, next->name, 0))
      continue;

    fprintf(stderr, "\n%s\n\n", next->name);

    for (x= 0; run->name; run++)
    {
      unsigned int loop;
      memcached_st *memc;
      memcached_return rc;
      struct timeval start_time, end_time;
      long int load_time;

      if (wildcard && fnmatch(wildcard, run->name, 0))
        continue;

      fprintf(stderr, "Testing %s", run->name);

      memc= memcached_create(NULL);
      assert(memc);

      rc= memcached_server_push(memc, servers);
      assert(rc == MEMCACHED_SUCCESS);

      if (run->requires_flush)
      {
        memcached_flush(memc, 0);
        memcached_quit(memc);
      }

      for (loop= 0; loop < memcached_server_list_count(servers); loop++)
      {
        assert(memc->hosts[loop].fd == -1);
        assert(memc->hosts[loop].cursor_active == 0);
      }

      if (next->pre)
      {
        rc= next->pre(memc);

        if (rc != MEMCACHED_SUCCESS)
        {
          fprintf(stderr, "\t\t\t\t\t [ skipping ]\n");
          goto error;
        }
      }

      gettimeofday(&start_time, NULL);
      failed= run->function(memc);
      gettimeofday(&end_time, NULL);
      load_time= timedif(end_time, start_time);

      fprintf(stderr, "\t\t\t\t\t %ld.%03ld [ %s ]\n", load_time / 1000,
              load_time % 1000, test_strerror(failed));

      if (next->post)
        (void)next->post(memc);

      assert(memc);
error:
      memcached_free(memc);
    }
  }

  fprintf(stderr, "All tests completed successfully\n\n");

  if (world.destroy)
    world.destroy(world_ptr);

  return 0;
}
