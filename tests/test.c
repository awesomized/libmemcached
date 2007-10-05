/*
  Sample test application.
*/
#include <assert.h>
#include <memcached.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_test(memcached_st *not_used)
{
  memcached_st memc;

  (void)memcached_create(&memc);
  memcached_free(&memc);
}

void allocation_test(memcached_st *not_used)
{
  memcached_st *memc;
  memc= memcached_create(NULL);
  assert(memc);
  memcached_free(memc);
}

void connection_test(memcached_st *memc)
{
  memcached_return rc;

  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);
}

void error_test(memcached_st *memc)
{
  memcached_return rc;

  for (rc= MEMCACHED_SUCCESS; rc < MEMCACHED_MAXIMUM_RETURN; rc++)
  {
    printf("Error %d -> %s\n", rc, memcached_strerror(memc, rc));
  }
}

void set_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);
}

void add_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_add(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_NOTSTORED);
}

void replace_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_replace(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);
}

void delete_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_delete(memc, key, strlen(key), (time_t)0);
  assert(rc == MEMCACHED_SUCCESS);
}

void flush_test(memcached_st *memc)
{
  memcached_return rc;

  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);
}

void get_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *string;
  size_t string_length;
  uint16_t flags;

  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(rc == MEMCACHED_NOTFOUND);
  assert(string_length ==  0);
  assert(!string);
}

void get_test2(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";
  char *string;
  size_t string_length;
  uint16_t flags;

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(string);
  assert(rc == MEMCACHED_SUCCESS);
  assert(string_length == strlen(value));
  assert(!memcmp(string, value, string_length));

  free(string);
}

void set_test2(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "train in the brain";
  size_t value_length= strlen(value);
  unsigned int x;

  for (x= 0; x < 10; x++)
  {
    rc= memcached_set(memc, key, strlen(key), 
                      value, value_length,
                      (time_t)0, (uint16_t)0);
    assert(rc == MEMCACHED_SUCCESS);
  }
}

void set_test3(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value;
  size_t value_length= 8191;
  unsigned int x;

  value = (char*)malloc(value_length);
  assert(value);

  for (x= 0; x < value_length; x++)
    value[x] = (char) (x % 127);

  for (x= 0; x < 1; x++)
  {
    rc= memcached_set(memc, key, strlen(key), 
                      value, value_length,
                      (time_t)0, (uint16_t)0);
    assert(rc == MEMCACHED_SUCCESS);
  }

  free(value);
}

void get_test3(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value;
  size_t value_length= 8191;
  char *string;
  size_t string_length;
  uint16_t flags;
  int x;

  value = (char*)malloc(value_length);
  assert(value);

  for (x= 0; x < value_length; x++)
    value[x] = (char) (x % 127);

  rc= memcached_set(memc, key, strlen(key), 
                    value, value_length,
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(rc == MEMCACHED_SUCCESS);
  assert(string);
  assert(string_length == value_length);
  assert(!memcmp(string, value, string_length));

  free(string);
  free(value);
}

void get_test4(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value;
  size_t value_length= 8191;
  char *string;
  size_t string_length;
  uint16_t flags;
  int x;

  value = (char*)malloc(value_length);
  assert(value);

  for (x= 0; x < value_length; x++)
    value[x] = (char) (x % 127);

  rc= memcached_set(memc, key, strlen(key), 
                    value, value_length,
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  for (x= 0; x < 10; x++)
  {
    string= memcached_get(memc, key, strlen(key),
                          &string_length, &flags, &rc);

    assert(rc == MEMCACHED_SUCCESS);
    assert(string);
    assert(string_length == value_length);
    assert(!memcmp(string, value, string_length));
    free(string);
  }

  free(value);
}

void stats_servername_test(memcached_st *memc)
{
  memcached_return rc;
  memcached_stat_st stat;
  rc= memcached_stat_servername(&stat, NULL,
                                 "localhost", 
                                 MEMCACHED_DEFAULT_PORT);
}

void increment_test(memcached_st *memc)
{
  unsigned int new_number;
  memcached_return rc;
  char *key= "number";
  char *value= "0";

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_increment(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 1);

  rc= memcached_increment(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 2);
}

void decrement_test(memcached_st *memc)
{
  unsigned int new_number;
  memcached_return rc;
  char *key= "number";
  char *value= "3";

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_decrement(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 2);

  rc= memcached_decrement(memc, key, strlen(key),
                          1, &new_number);
  assert(rc == MEMCACHED_SUCCESS);
  assert(new_number == 1);
}

void quit_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "fudge";
  char *value= "sanford and sun";

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)10, (uint16_t)3);
  assert(rc == MEMCACHED_SUCCESS);
  memcached_quit(memc);

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)50, (uint16_t)9);
  assert(rc == MEMCACHED_SUCCESS);
}

void mget_test(memcached_st *memc)
{
  memcached_return rc;
  char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};
  unsigned int x;
  uint16_t flags;

  char return_key[MEMCACHED_MAX_KEY];
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;

  /* We need to empty the server before continueing test */
  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_mget(memc, keys, key_length, 3);
  assert(rc == MEMCACHED_SUCCESS);

  while ((return_value= memcached_fetch(memc, return_key, &return_key_length, 
                      &return_value_length, &flags, &rc)) != NULL)
  {
    assert(return_value);
  }
  assert(!return_value);
  assert(return_value_length == 0);
  assert(rc == MEMCACHED_NOTFOUND);

  for (x= 0; x < 3; x++)
  {
    rc= memcached_set(memc, keys[x], key_length[x], 
                      keys[x], key_length[x],
                      (time_t)50, (uint16_t)9);
    assert(rc == MEMCACHED_SUCCESS);
  }

  rc= memcached_mget(memc, keys, key_length, 3);
  assert(rc == MEMCACHED_SUCCESS);

  x= 0;
  while ((return_value= memcached_fetch(memc, return_key, &return_key_length, 
                                        &return_value_length, &flags, &rc)))
  {
    assert(return_value);
    assert(rc == MEMCACHED_SUCCESS);
    assert(return_key_length == return_value_length);
    assert(!memcmp(return_value, return_key, return_value_length));
    free(return_value);
    x++;
  }
}

void get_stats_keys(memcached_st *memc)
{
 char **list;
 char **ptr;
 memcached_stat_st stat;
 memcached_return rc;

 list= memcached_stat_get_keys(memc, &stat, &rc);
 assert(rc == MEMCACHED_SUCCESS);
 for (ptr= list; *ptr; ptr++)
   printf("Found key %s\n", *ptr);

 free(list);
}

void get_stats(memcached_st *memc)
{
 unsigned int x;
 char **list;
 char **ptr;
 memcached_return rc;
 memcached_stat_st *stat;

 stat= memcached_stat(memc, NULL, &rc);
 assert(rc == MEMCACHED_SUCCESS);

 assert(rc == MEMCACHED_SUCCESS);
 assert(stat);

 for (x= 0; x < memcached_server_count(memc); x++)
 {
   list= memcached_stat_get_keys(memc, &stat[x], &rc);
   assert(rc == MEMCACHED_SUCCESS);
   for (ptr= list; *ptr; ptr++)
     printf("Found key %s\n", *ptr);

   free(list);
 }

 free(stat);
}

void add_host_test(memcached_st *memc)
{
  unsigned int x;
  memcached_server_st *servers;
  memcached_return rc;
  char servername[]= "0.example.com";

  servers= memcached_server_list_append(NULL, servername, 400, &rc);
  assert(servers);
  assert(1 == memcached_server_list_count(servers));

  for (x= 2; x < 20; x++)
  {
    char buffer[SMALL_STRING_LEN];

    snprintf(buffer, SMALL_STRING_LEN, "%u.example.com", 400+x);
    servers= memcached_server_list_append(servers, buffer, 401, 
                                     &rc);
    assert(rc == MEMCACHED_SUCCESS);
    assert(x == memcached_server_list_count(servers));
  }

  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);

  memcached_server_list_free(servers);
}

void add_host_test1(memcached_st *memc)
{
  unsigned int x;
  memcached_return rc;
  char servername[]= "0.example.com";
  memcached_server_st *servers;

  servers= memcached_server_list_append(NULL, servername, 400, &rc);
  assert(servers);
  assert(1 == memcached_server_list_count(servers));

  for (x= 2; x < 20; x++)
  {
    char buffer[SMALL_STRING_LEN];

    snprintf(buffer, SMALL_STRING_LEN, "%u.example.com", 400+x);
    servers= memcached_server_list_append(servers, buffer, 401, 
                                     &rc);
    assert(rc == MEMCACHED_SUCCESS);
    assert(x == memcached_server_list_count(servers));
  }

  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);
  rc= memcached_server_push(memc, servers);
  assert(rc == MEMCACHED_SUCCESS);

  memcached_server_list_free(servers);
}

typedef struct test_st test_st;

struct test_st {
  char *function_name;
  unsigned int requires_flush;
  void (*function)(memcached_st *memc);
};

int main(int argc, char *argv[])
{
  unsigned int x;
  char *server_list;
  memcached_server_st *servers;

  if (!(server_list= getenv("MEMCACHED_SERVERS")))
    server_list= "localhost";

  printf("servers %s\n", server_list);

  servers= memcached_servers_parse(server_list);
  assert(servers);

  /* Clean the server before beginning testing */
  test_st tests[] ={
    {"flush", 0, flush_test },
    {"init", 0, init_test },
    {"allocation", 0, allocation_test },
    {"error", 0, error_test },
    {"set", 0, set_test },
    {"set2", 0, set_test2 },
    {"set3", 0, set_test3 },
    {"add", 0, add_test },
    {"replace", 0, replace_test },
    {"delete", 1, delete_test },
    {"get", 0, get_test },
    {"get2", 0, get_test2 },
    {"get3", 0, get_test3 },
    {"get4", 0, get_test4 },
    {"stats_servername", 0, stats_servername_test },
    {"increment", 0, increment_test },
    {"decrement", 0, decrement_test },
    {"quit", 0, quit_test },
    {"mget", 0, mget_test },
    {"get_stats", 0, get_stats },
    {"add_host_test", 0, add_host_test },
    {"get_stats_keys", 0, get_stats_keys },
    {0, 0, 0}
  };

  fprintf(stderr, "\nBlock tests\n\n");
  for (x= 0; tests[x].function_name; x++)
  {
    memcached_st *memc;
    memcached_return rc;
    memc= memcached_create(NULL);
    assert(memc);

    rc= memcached_server_push(memc, servers);
    assert(rc == MEMCACHED_SUCCESS);

    fprintf(stderr, "Testing %s", tests[x].function_name);
    tests[x].function(memc);
    fprintf(stderr, "\t\t\t\t\t[ ok ]\n");
    assert(memc);
    memcached_free(memc);
  }

  fprintf(stderr, "\nNonblock tests\n\n");
  for (x= 0; tests[x].function_name; x++)
  {
    memcached_st *memc;
    memcached_return rc;
    memc= memcached_create(NULL);
    assert(memc);

    rc= memcached_server_push(memc, servers);
    assert(rc == MEMCACHED_SUCCESS);

    fprintf(stderr, "Testing %s", tests[x].function_name);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, NULL);
    tests[x].function(memc);
    fprintf(stderr, "\t\t\t\t\t[ ok ]\n");
    assert(memc);
    memcached_free(memc);
  }

  fprintf(stderr, "\nTCP Nodelay tests\n\n");
  for (x= 0; tests[x].function_name; x++)
  {
    memcached_st *memc;
    memcached_return rc;
    memc= memcached_create(NULL);
    assert(memc);

    rc= memcached_server_push(memc, servers);
    assert(rc == MEMCACHED_SUCCESS);

    fprintf(stderr, "Testing %s", tests[x].function_name);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, NULL);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, NULL);
    tests[x].function(memc);
    fprintf(stderr, "\t\t\t\t\t[ ok ]\n");
    assert(memc);
    memcached_free(memc);
  }

  fprintf(stderr, "\nMD5 Hashing\n\n");
  for (x= 0; tests[x].function_name; x++)
  {
    memcached_st *memc;
    memcached_return rc;
    memc= memcached_create(NULL);
    assert(memc);

    rc= memcached_server_push(memc, servers);
    assert(rc == MEMCACHED_SUCCESS);

    fprintf(stderr, "Testing %s", tests[x].function_name);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_MD5_HASHING, NULL);
    tests[x].function(memc);
    fprintf(stderr, "\t\t\t\t\t[ ok ]\n");
    assert(memc);
    memcached_free(memc);
  }

  /* Clean up whatever we might have left */
  {
    memcached_st *memc;
    memc= memcached_create(NULL);
    assert(memc);
    flush_test(memc);
    memcached_free(memc);
  }
  return 0;
}
