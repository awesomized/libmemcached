/*
  Sample test application.
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "test.h"

long int timedif(struct timeval a, struct timeval b)
{
  register int us, s;

  us = a.tv_usec - b.tv_usec;
  us /= 1000;
  s = a.tv_sec - b.tv_sec;
  s *= 1000;
  return s + us;
}

int main(int argc, char *argv[])
{
  unsigned int x;
  char *server_list;
  char *collection_to_run= NULL;
  char *wildcard= NULL;
  memcached_server_st *servers;
  collection_st *collection;
  uint8_t failed;

  collection= gets_collections();


  if (argc > 1)
    collection_to_run= argv[1];

  if (argc == 3)
    wildcard= argv[2];

  if (!(server_list= getenv("MEMCACHED_SERVERS")))
    server_list= "localhost";

  printf("servers %s\n", server_list);
  srandom(time(NULL));

  servers= memcached_servers_parse(server_list);
  assert(servers);

  for (x= 0; x < memcached_server_list_count(servers); x++)
  {
    printf("\t%s : %u\n", servers[x].hostname, servers[x].port);
    assert(servers[x].stack_responses == 0);
    assert(servers[x].fd == -1);
    assert(servers[x].cursor_active == 0);
  }

  printf("\n");

  collection_st *next;
  for (next= collection; next->name; next++)
  {
    test_st *run;

    run= next->tests;
    if (collection_to_run && strcmp(collection_to_run, next->name))
      continue;

    fprintf(stderr, "\n%s\n\n", next->name);

    for (x= 0; run->name; run++)
    {
      if (wildcard && strcmp(wildcard, run->name))
        continue;

      fprintf(stderr, "Testing %s", run->name);

      memcached_st *memc;
      memcached_return rc;
      struct timeval start_time, end_time;

      memc= memcached_create(NULL);
      assert(memc);

      if (run->requires_flush)
        memcached_flush(memc, 0);

      rc= memcached_server_push(memc, servers);
      assert(rc == MEMCACHED_SUCCESS);

      unsigned int loop;
      for (loop= 0; loop < memcached_server_list_count(servers); loop++)
      {
        assert(memc->hosts[loop].stack_responses == 0);
        assert(memc->hosts[loop].fd == -1);
        assert(memc->hosts[loop].cursor_active == 0);
      }

      if (next->pre)
      {
        memcached_return rc;
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
      long int load_time= timedif(end_time, start_time);
      if (failed)
        fprintf(stderr, "\t\t\t\t\t %ld.%03ld [ failed ]\n", load_time / 1000, 
                load_time % 1000);
      else
        fprintf(stderr, "\t\t\t\t\t %ld.%03ld [ ok ]\n", load_time / 1000, 
                load_time % 1000);

      if (next->post)
        (void)next->post(memc);

      assert(memc);
error:
      memcached_free(memc);
    }
  }

  fprintf(stderr, "All tests completed successfully\n\n");

  memcached_server_list_free(servers);

  return 0;
}
