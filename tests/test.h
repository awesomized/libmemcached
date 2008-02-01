/*
  Structures for generic tests.
*/
#ifdef	__cplusplus
extern "C" {

#endif
#include <memcached.h>
#include "../lib/common.h"

typedef struct world_st world_st;
typedef struct collection_st collection_st;
typedef struct test_st test_st;

struct test_st {
  char *name;
  unsigned int requires_flush;
  uint8_t (*function)(memcached_st *memc);
};

struct collection_st {
  char *name;
  memcached_return (*pre)(memcached_st *memc);
  memcached_return (*post)(memcached_st *memc);
  test_st *tests;
};

struct world_st {
  collection_st *collections;
  void *(*create)(void);
  void (*destroy)(void *collection_object);
};

/* How we make all of this work :) */
void get_world(world_st *world);

#ifdef	__cplusplus
}
#endif
