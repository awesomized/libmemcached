/*
  Sample test application.
*/
#include <assert.h>
#include <memcached.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "../lib/common.h"
#include "../src/generator.h"
#include "../src/execute.h"

#include "test.h"

#define GLOBAL_COUNT 100000
static pairs_st *global_pairs;
static char *global_keys[GLOBAL_COUNT];
static size_t global_keys_length[GLOBAL_COUNT];

uint8_t init_test(memcached_st *not_used)
{
  memcached_st memc;

  (void)memcached_create(&memc);
  memcached_free(&memc);

  return 0;
}

uint8_t allocation_test(memcached_st *not_used)
{
  memcached_st *memc;
  memc= memcached_create(NULL);
  assert(memc);
  memcached_free(memc);

  return 0;
}

uint8_t clone_test(memcached_st *memc)
{
  /* All null? */
  {
    memcached_st *clone;
    clone= memcached_clone(NULL, NULL);
    assert(clone);
    memcached_free(clone);
  }

  /* Can we init from null? */
  {
    memcached_st *clone;
    clone= memcached_clone(NULL, memc);
    assert(clone);
    memcached_free(clone);
  }

  /* Can we init from struct? */
  {
    memcached_st declared_clone;
    memcached_st *clone;
    clone= memcached_clone(&declared_clone, NULL);
    assert(clone);
    memcached_free(clone);
  }

  /* Can we init from struct? */
  {
    memcached_st declared_clone;
    memcached_st *clone;
    clone= memcached_clone(&declared_clone, memc);
    assert(clone);
    memcached_free(clone);
  }

  return 0;
}

uint8_t connection_test(memcached_st *memc)
{
  memcached_return rc;

  rc= memcached_server_add(memc, "localhost", 0);
  assert(rc == MEMCACHED_SUCCESS);

  return 0;
}

uint8_t error_test(memcached_st *memc)
{
  memcached_return rc;

  for (rc= MEMCACHED_SUCCESS; rc < MEMCACHED_MAXIMUM_RETURN; rc++)
  {
    printf("Error %d -> %s\n", rc, memcached_strerror(memc, rc));
  }

  return 0;
}

uint8_t set_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_set(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  return 0;
}

uint8_t add_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_add(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_NOTSTORED);

  return 0;
}

uint8_t replace_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *value= "when we sanitize";

  rc= memcached_replace(memc, key, strlen(key), 
                    value, strlen(value),
                    (time_t)0, (uint16_t)0);
  assert(rc == MEMCACHED_SUCCESS);

  return 0;
}

uint8_t delete_test(memcached_st *memc)
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

  return 0;
}

uint8_t flush_test(memcached_st *memc)
{
  memcached_return rc;

  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);

  return 0;
}

uint8_t get_test(memcached_st *memc)
{
  memcached_return rc;
  char *key= "foo";
  char *string;
  size_t string_length;
  uint16_t flags;

  rc= memcached_delete(memc, key, strlen(key), (time_t)0);
  assert(rc == MEMCACHED_SUCCESS || rc == MEMCACHED_NOTFOUND);

  string= memcached_get(memc, key, strlen(key),
                        &string_length, &flags, &rc);

  assert(rc == MEMCACHED_NOTFOUND);
  assert(string_length ==  0);
  assert(!string);

  return 0;
}

uint8_t get_test2(memcached_st *memc)
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

  return 0;
}

uint8_t set_test2(memcached_st *memc)
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

  return 0;
}

uint8_t set_test3(memcached_st *memc)
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

  return 0;
}

uint8_t get_test3(memcached_st *memc)
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

  return 0;
}

uint8_t get_test4(memcached_st *memc)
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

  return 0;
}

uint8_t stats_servername_test(memcached_st *memc)
{
  memcached_return rc;
  memcached_stat_st stat;
  rc= memcached_stat_servername(&stat, NULL,
                                 "localhost", 
                                 MEMCACHED_DEFAULT_PORT);

  return 0;
}

uint8_t increment_test(memcached_st *memc)
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

  return 0;
}

uint8_t decrement_test(memcached_st *memc)
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

  return 0;
}

uint8_t quit_test(memcached_st *memc)
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

  return 0;
}

uint8_t mget_result_test(memcached_st *memc)
{
  memcached_return rc;
  char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};
  unsigned int x;

  memcached_result_st results_obj;
  memcached_result_st *results;

  results= memcached_result_create(memc, &results_obj);
  assert(results);
  assert(&results_obj == results);

  /* We need to empty the server before continueing test */
  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_mget(memc, keys, key_length, 3);
  assert(rc == MEMCACHED_SUCCESS);

  while ((results= memcached_fetch_result(memc, &results_obj, &rc)) != NULL)
  {
    assert(results);
  }
  while ((results= memcached_fetch_result(memc, &results_obj, &rc)) != NULL)
  assert(!results);
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

  while ((results= memcached_fetch_result(memc, &results_obj, &rc)))
  {
    assert(results);
    assert(&results_obj == results);
    assert(rc == MEMCACHED_SUCCESS);
    assert(memcached_result_key_length(results) == memcached_result_length(results));
    assert(!memcmp(memcached_result_key_value(results), 
                   memcached_result_value(results), 
                   memcached_result_length(results)));
  }

  memcached_result_free(&results_obj);

  return 0;
}

uint8_t mget_result_alloc_test(memcached_st *memc)
{
  memcached_return rc;
  char *keys[]= {"fudge", "son", "food"};
  size_t key_length[]= {5, 3, 4};
  unsigned int x;

  memcached_result_st *results;

  /* We need to empty the server before continueing test */
  rc= memcached_flush(memc, 0);
  assert(rc == MEMCACHED_SUCCESS);

  rc= memcached_mget(memc, keys, key_length, 3);
  assert(rc == MEMCACHED_SUCCESS);

  while ((results= memcached_fetch_result(memc, NULL, &rc)) != NULL)
  {
    assert(results);
  }
  assert(!results);
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
  while ((results= memcached_fetch_result(memc, NULL, &rc)))
  {
    assert(results);
    assert(rc == MEMCACHED_SUCCESS);
    assert(memcached_result_key_length(results) == memcached_result_length(results));
    assert(!memcmp(memcached_result_key_value(results), 
                   memcached_result_value(results), 
                   memcached_result_length(results)));
    memcached_result_free(results);
    x++;
  }

  return 0;
}

uint8_t mget_test(memcached_st *memc)
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

  return 0;
}

uint8_t get_stats_keys(memcached_st *memc)
{
 char **list;
 char **ptr;
 memcached_stat_st stat;
 memcached_return rc;

 list= memcached_stat_get_keys(memc, &stat, &rc);
 assert(rc == MEMCACHED_SUCCESS);
 for (ptr= list; *ptr; ptr++)
   printf("Found key %s\n", *ptr);
 fflush(stdout);

 free(list);

 return 0;
}

uint8_t get_stats(memcached_st *memc)
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

 memcached_stat_free(NULL, stat);

  return 0;
}

uint8_t add_host_test(memcached_st *memc)
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

  return 0;
}

/* We don't test the behavior itself, we test the switches */
uint8_t behavior_test(memcached_st *memc)
{
  unsigned long long value;
  unsigned int set= 1;

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_NO_BLOCK);
  assert(value == 1);

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY);
  assert(value == 1);

  set= MEMCACHED_HASH_MD5;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_HASH);
  assert(value == MEMCACHED_HASH_MD5);

  set= 0;

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_NO_BLOCK);
  assert(value == 0);

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY);
  assert(value == 0);

  set= MEMCACHED_HASH_DEFAULT;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_HASH);
  assert(value == MEMCACHED_HASH_DEFAULT);

  set= MEMCACHED_HASH_CRC;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &set);
  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_HASH);
  assert(value == MEMCACHED_HASH_CRC);

  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE);
  assert(value > 0);

  value= memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE);
  assert(value > 0);

  return 0;
}

/* Test case provided by Cal Haldenbrand */
uint8_t user_supplied_bug1(memcached_st *memc)
{
  unsigned int setter= 1;
  unsigned int x;

  unsigned long long total= 0;
  int size= 0;
  char key[10];
  char randomstuff[6 * 1024]; 
  memcached_return rc;

  memset(randomstuff, 0, 6 * 1024);

  /* We just keep looking at the same values over and over */
  srandom(10);

  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, &setter);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, &setter);


  /* add key */
  for (x= 0 ; total < 20 * 1024576 ; x++ )
  {
    unsigned int j= 0;

    size= (rand() % ( 5 * 1024 ) ) + 400;
    memset(randomstuff, 0, 6 * 1024);
    assert(size < 6 * 1024); /* Being safe here */

    for (j= 0 ; j < size ;j++) 
      randomstuff[j] = (char) (rand() % 26) + 97;

    total += size;
    sprintf(key, "%d", x);
    rc = memcached_set(memc, key, strlen(key), 
                       randomstuff, strlen(randomstuff), 10, 0);
    /* If we fail, lets try again */
    if (rc != MEMCACHED_SUCCESS)
      rc = memcached_set(memc, key, strlen(key), 
                         randomstuff, strlen(randomstuff), 10, 0);
    assert(rc == MEMCACHED_SUCCESS);
  }

  return 0;
}

/* Test case provided by Cal Haldenbrand */
uint8_t user_supplied_bug2(memcached_st *memc)
{
  int errors;
  unsigned int setter;
  unsigned int x;
  unsigned long long total;

  setter= 1;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, &setter);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, &setter);
#ifdef NOT_YET
  setter = 20 * 1024576;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE, &setter);
  setter = 20 * 1024576;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE, &setter);
  getter = memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE);
  getter = memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE);

  for (x= 0, errors= 0, total= 0 ; total < 20 * 1024576 ; x++)
#endif

  for (x= 0, errors= 0, total= 0 ; total < 24576 ; x++)
  {
    memcached_return rc= MEMCACHED_SUCCESS;
    char buffer[SMALL_STRING_LEN];
    uint16_t flags= 0;
    size_t val_len= 0;
    char *getval;

    memset(buffer, 0, SMALL_STRING_LEN);

    snprintf(buffer, SMALL_STRING_LEN, "%u", x);
    getval= memcached_get(memc, buffer, strlen(buffer),
                           &val_len, &flags, &rc);		
    if (rc != MEMCACHED_SUCCESS) 
    {
      if (rc == MEMCACHED_NOTFOUND)
        errors++;
      else
        assert(0);

      continue;
    }
    total+= val_len;
    errors= 0;
    free(getval);
  }

  return 0;
}

/* Do a large mget() over all the keys we think exist */
#define KEY_COUNT 3000 // * 1024576
uint8_t user_supplied_bug3(memcached_st *memc)
{
  memcached_return rc;
  unsigned int setter;
  unsigned int x;
  char **keys;
  size_t key_lengths[KEY_COUNT];

  setter= 1;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, &setter);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, &setter);
#ifdef NOT_YET
  setter = 20 * 1024576;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE, &setter);
  setter = 20 * 1024576;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE, &setter);
  getter = memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_SOCKET_SEND_SIZE);
  getter = memcached_behavior_get(memc, MEMCACHED_BEHAVIOR_SOCKET_RECV_SIZE);
#endif

  keys= (char **)malloc(sizeof(char *) * KEY_COUNT);
  assert(keys);
  memset(keys, 0, (sizeof(char *) * KEY_COUNT));
  for (x= 0; x < KEY_COUNT; x++)
  {
    char buffer[30];

    snprintf(buffer, 30, "%u", x);
    keys[x]= strdup(buffer);
    key_lengths[x]= strlen(keys[x]);
  }

  rc= memcached_mget(memc, keys, key_lengths, KEY_COUNT);
  assert(rc == MEMCACHED_SUCCESS);

  /* Turn this into a help function */
  {
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    uint16_t flags;

    while ((return_value= memcached_fetch(memc, return_key, &return_key_length, 
                                          &return_value_length, &flags, &rc)))
    {
      assert(return_value);
      assert(rc == MEMCACHED_SUCCESS);
      free(return_value);
    }
  }

  for (x= 0; x < KEY_COUNT; x++)
    free(keys[x]);
  free(keys);

  return 0;
}

uint8_t result_static(memcached_st *memc)
{
  memcached_result_st result;
  memcached_result_st *result_ptr;

  result_ptr= memcached_result_create(memc, &result);
  assert(result.is_allocated == MEMCACHED_NOT_ALLOCATED);
  assert(result_ptr);
  memcached_result_free(&result);

  return 0;
}

uint8_t result_alloc(memcached_st *memc)
{
  memcached_result_st *result;

  result= memcached_result_create(memc, NULL);
  assert(result);
  memcached_result_free(result);

  return 0;
}

uint8_t string_static_null(memcached_st *memc)
{
  memcached_string_st string;
  memcached_string_st *string_ptr;

  string_ptr= memcached_string_create(memc, &string, 0);
  assert(string.is_allocated == MEMCACHED_NOT_ALLOCATED);
  assert(string_ptr);
  memcached_string_free(&string);

  return 0;
}

uint8_t string_alloc_null(memcached_st *memc)
{
  memcached_string_st *string;

  string= memcached_string_create(memc, NULL, 0);
  assert(string);
  memcached_string_free(string);

  return 0;
}

uint8_t string_alloc_with_size(memcached_st *memc)
{
  memcached_string_st *string;

  string= memcached_string_create(memc, NULL, 1024);
  assert(string);
  memcached_string_free(string);

  return 0;
}

uint8_t string_alloc_with_size_toobig(memcached_st *memc)
{
  memcached_string_st *string;

  string= memcached_string_create(memc, NULL, INT64_MAX);
  assert(string == NULL);

  return 0;
}

uint8_t string_alloc_append(memcached_st *memc)
{
  unsigned int x;
  char buffer[SMALL_STRING_LEN];
  memcached_string_st *string;

  /* Ring the bell! */
  memset(buffer, 6, SMALL_STRING_LEN);

  string= memcached_string_create(memc, NULL, 100);
  assert(string);

  for (x= 0; x < 1024; x++)
  {
    memcached_return rc;
    rc= memcached_string_append(string, buffer, SMALL_STRING_LEN);
    assert(rc == MEMCACHED_SUCCESS);
  }
  memcached_string_free(string);

  return 0;
}

uint8_t string_alloc_append_toobig(memcached_st *memc)
{
  memcached_return rc;
  unsigned int x;
  char buffer[SMALL_STRING_LEN];
  memcached_string_st *string;

  /* Ring the bell! */
  memset(buffer, 6, SMALL_STRING_LEN);

  string= memcached_string_create(memc, NULL, 100);
  assert(string);

  for (x= 0; x < 1024; x++)
  {
    rc= memcached_string_append(string, buffer, SMALL_STRING_LEN);
    assert(rc == MEMCACHED_SUCCESS);
  }
  rc= memcached_string_append(string, buffer, INT64_MAX);
  assert(rc == MEMCACHED_MEMORY_ALLOCATION_FAILURE);
  memcached_string_free(string);

  return 0;
}

uint8_t generate_data(memcached_st *memc)
{
  unsigned long long x;
  global_pairs= pairs_generate(GLOBAL_COUNT);
  execute_set(memc, global_pairs, GLOBAL_COUNT);

  for (x= 0; x < GLOBAL_COUNT; x++)
  {
    global_keys[x]= global_pairs[x].key; 
    global_keys_length[x]=  global_pairs[x].key_length;
  }

  return 0;
}

uint8_t get_read(memcached_st *memc)
{
  unsigned int x;
  memcached_return rc;

  {
    char *return_value;
    size_t return_value_length;
    uint16_t flags;

    for (x= 0; x < GLOBAL_COUNT; x++)
    {
      return_value= memcached_get(memc, global_keys[x], global_keys_length[x],
                                  &return_value_length, &flags, &rc);
      /*
      assert(return_value);
      assert(rc == MEMCACHED_SUCCESS);
    */
      if (rc == MEMCACHED_SUCCESS && return_value)
        free(return_value);
    }
  }

  return 0;
}

uint8_t mget_read(memcached_st *memc)
{
  memcached_return rc;

  rc= memcached_mget(memc, global_keys, global_keys_length, GLOBAL_COUNT);
  assert(rc == MEMCACHED_SUCCESS);
  /* Turn this into a help function */
  {
    char return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char *return_value;
    size_t return_value_length;
    uint16_t flags;

    while ((return_value= memcached_fetch(memc, return_key, &return_key_length,
                                          &return_value_length, &flags, &rc)))
    {
      assert(return_value);
      assert(rc == MEMCACHED_SUCCESS);
      free(return_value);
    }
  }

  return 0;
}

uint8_t mget_read_result(memcached_st *memc)
{
  memcached_return rc;

  rc= memcached_mget(memc, global_keys, global_keys_length, GLOBAL_COUNT);
  assert(rc == MEMCACHED_SUCCESS);
  /* Turn this into a help function */
  {
    memcached_result_st results_obj;
    memcached_result_st *results;

    results= memcached_result_create(memc, &results_obj);

    while ((results= memcached_fetch_result(memc, &results_obj, &rc)))
    {
      assert(results);
      assert(rc == MEMCACHED_SUCCESS);
    }

    memcached_result_free(&results_obj);
  }

  return 0;
}

uint8_t free_data(memcached_st *memc)
{
  pairs_free(global_pairs);

  return 0;
}

uint8_t add_host_test1(memcached_st *memc)
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

  return 0;
}

memcached_return pre_nonblock(memcached_st *memc)
{
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, NULL);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_md5(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_MD5;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_crc(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_CRC;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_hash_fnv1_64(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_FNV1_64;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_hash_fnv1a_64(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_FNV1A_64;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_hash_fnv1_32(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_FNV1_32;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_hash_fnv1a_32(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_FNV1A_32;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_hash_ketama(memcached_st *memc)
{
  memcached_hash value= MEMCACHED_HASH_KETAMA;
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, &value);

  return MEMCACHED_SUCCESS;
}

memcached_return pre_unix_socket(memcached_st *memc)
{
  memcached_return rc;
  struct stat buf;

  memcached_server_list_free(memc->hosts);
  memc->hosts= NULL;
  memc->number_of_hosts= 0;

  if (stat("/tmp/memcached.socket", &buf))
    return MEMCACHED_FAILURE;

  rc= memcached_server_add_unix_socket(memc, "/tmp/memcached.socket");

  return rc;
}

memcached_return pre_nodelay(memcached_st *memc)
{
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, NULL);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_TCP_NODELAY, NULL);

  return MEMCACHED_SUCCESS;
}


/* Clean the server before beginning testing */
test_st tests[] ={
  {"flush", 0, flush_test },
  {"init", 0, init_test },
  {"allocation", 0, allocation_test },
  {"clone_test", 0, clone_test },
  {"error", 0, error_test },
  {"set", 0, set_test },
  {"set2", 0, set_test2 },
  {"set3", 0, set_test3 },
  {"add", 0, add_test },
  {"replace", 0, replace_test },
  {"delete", 1, delete_test },
  {"get", 1, get_test },
  {"get2", 0, get_test2 },
  {"get3", 0, get_test3 },
  {"get4", 0, get_test4 },
  {"stats_servername", 0, stats_servername_test },
  {"increment", 0, increment_test },
  {"decrement", 0, decrement_test },
  {"quit", 0, quit_test },
  {"mget", 1, mget_test },
  {"mget_result", 1, mget_result_test },
  {"mget_result_alloc", 1, mget_result_alloc_test },
  {"get_stats", 0, get_stats },
  {"add_host_test", 0, add_host_test },
  {"get_stats_keys", 0, get_stats_keys },
  {"behavior_test", 0, get_stats_keys },
  {0, 0, 0}
};

test_st string_tests[] ={
  {"string static with null", 0, string_static_null },
  {"string alloc with null", 0, string_alloc_null },
  {"string alloc with 1K", 0, string_alloc_with_size },
  {"string alloc with malloc failure", 0, string_alloc_with_size_toobig },
  {"string append", 0, string_alloc_append },
  {"string append failure (too big)", 0, string_alloc_append_toobig },
  {0, 0, 0}
};

test_st result_tests[] ={
  {"result static", 0, result_static},
  {"result alloc", 0, result_alloc},
  {0, 0, 0}
};

test_st user_tests[] ={
  {"user_supplied_bug1", 0, user_supplied_bug1 },
  {"user_supplied_bug2", 0, user_supplied_bug2 },
  {"user_supplied_bug3", 0, user_supplied_bug3 },
  {0, 0, 0}
};

test_st generate_tests[] ={
  {"generate_data", 0, generate_data },
  {"get_read", 0, get_read },
  {"mget_read", 0, mget_read },
  {"mget_read_result", 0, mget_read_result },
  {0, 0, 0}
};


collection_st collection[] ={
  {"block", 0, 0, tests},
  {"nonblock", pre_nonblock, 0, tests},
  {"nodelay", pre_nodelay, 0, tests},
  {"md5", pre_md5, 0, tests},
  {"crc", pre_crc, 0, tests},
  {"fnv1_64", pre_hash_fnv1_64, 0, tests},
  {"fnv1a_64", pre_hash_fnv1a_64, 0, tests},
  {"fnv1_32", pre_hash_fnv1_32, 0, tests},
  {"fnv1a_32", pre_hash_fnv1a_32, 0, tests},
  {"ketama", pre_hash_ketama, 0, tests},
  {"unix_socket", pre_unix_socket, 0, tests},
  {"unix_socket_nodelay", pre_nodelay, 0, tests},
  {"string", 0, 0, string_tests},
  {"result", 0, 0, result_tests},
  {"user", 0, 0, user_tests},
  {"generate", 0, 0, generate_tests},
  {0, 0, 0, 0}
};

collection_st *gets_collections(void)
{
  return collection;
}
