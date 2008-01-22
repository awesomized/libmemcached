/*
  C++ interface test
*/
#include <assert.h>
#include <memcached.hh>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "test.h"

uint8_t basic_test(memcached_st *memc)
{
  Memcached foo;
  char *value_set= "This is some data";
  char *value;
  size_t value_length;

  foo.set("mine", value_set, strlen(value_set));
  value= foo.get("mine", &value_length);

  assert((memcmp(value, value_set, value_length) == 0));

  return 0;
}

test_st tests[] ={
  {"basic", 0, basic_test },
  {0, 0, 0}
};

collection_st collection[] ={
  {"block", 0, 0, tests},
  {0, 0, 0, 0}
};

collection_st *gets_collections(void)
{
  return collection;
}
