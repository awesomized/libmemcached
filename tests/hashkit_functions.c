/* libHashKit Functions Test
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libhashkit/hashkit.h>

#include "test.h"

#include "hash_results.h"

static hashkit_st global_hashk;

/**
  @brief hash_test_st is a structure we use in testing. It is currently empty.
*/
typedef struct hash_test_st hash_test_st;

struct hash_test_st
{
  bool _unused;
};

static test_return_t init_test(void *not_used __attribute__((unused)))
{
  hashkit_st hashk;
  hashkit_st *hashk_ptr;

  hashk_ptr= hashkit_create(&hashk);
  test_truth(hashk_ptr);
  test_truth(hashk_ptr == &hashk);
  test_truth(hashkit_is_initialized(&hashk) == true);
  test_truth(hashkit_is_allocated(hashk_ptr) == false);

  hashkit_free(hashk_ptr);

  test_truth(hashkit_is_initialized(&hashk) == false);

  return TEST_SUCCESS;
}

static test_return_t allocation_test(void *not_used __attribute__((unused)))
{
  hashkit_st *hashk_ptr;

  hashk_ptr= hashkit_create(NULL);
  test_truth(hashk_ptr);
  test_truth(hashkit_is_allocated(hashk_ptr) == true);
  test_truth(hashkit_is_initialized(hashk_ptr) == true);
  hashkit_free(hashk_ptr);

  return TEST_SUCCESS;
}

static test_return_t clone_test(hashkit_st *hashk)
{
  // First we make sure that the testing system is giving us what we expect.
  assert(&global_hashk == hashk);

  // Second we test if hashk is even valid
  test_truth(hashkit_is_initialized(hashk) == true);

  /* All null? */
  {
    hashkit_st *hashk_ptr;
    hashk_ptr= hashkit_clone(NULL, NULL);
    test_truth(hashk_ptr);
    test_truth(hashkit_is_allocated(hashk_ptr) == true);
    test_truth(hashkit_is_initialized(hashk_ptr) == true);
    hashkit_free(hashk_ptr);
  }

  /* Can we init from null? */
  {
    hashkit_st *hashk_ptr;

    hashk_ptr= hashkit_clone(NULL, hashk);

    test_truth(hashk_ptr);
    test_truth(hashkit_is_allocated(hashk_ptr) == true);
    test_truth(hashkit_is_initialized(hashk_ptr) == true);

    test_truth(hashk_ptr->distribution == hashk->distribution);
    test_truth(hashk_ptr->continuum_count == hashk->continuum_count);
    test_truth(hashk_ptr->continuum_points_count == hashk->continuum_points_count);
    test_truth(hashk_ptr->list_size == hashk->list_size);
    test_truth(hashk_ptr->context_size == hashk->context_size);
    test_truth(hashk_ptr->continuum == NULL);
    test_truth(hashk_ptr->hash_fn == hashk->hash_fn);
    test_truth(hashk_ptr->active_fn == hashk->active_fn);
    test_truth(hashk_ptr->continuum_hash_fn == hashk->continuum_hash_fn);
    test_truth(hashk_ptr->continuum_key_fn == hashk->continuum_key_fn);
    test_truth(hashk_ptr->sort_fn == hashk->sort_fn);
    test_truth(hashk_ptr->weight_fn == hashk->weight_fn);
    test_truth(hashk_ptr->list == hashk->list);

    hashkit_free(hashk_ptr);
  }

  /* Can we init from struct? */
  {
    hashkit_st declared_clone;
    hashkit_st *hash_clone;

    hash_clone= hashkit_clone(&declared_clone, NULL);
    test_truth(hash_clone);

    hashkit_free(hash_clone);
  }

  /* Can we init from struct? */
  {
    hashkit_st declared_clone;
    hashkit_st *hash_clone;
    memset(&declared_clone, 0 , sizeof(hashkit_st));
    hash_clone= hashkit_clone(&declared_clone, hashk);
    test_truth(hash_clone);
    hashkit_free(hash_clone);
  }

  return TEST_SUCCESS;
}


static test_return_t md5_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_md5(*ptr, strlen(*ptr));
    test_truth(md5_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t crc_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_crc32(*ptr, strlen(*ptr));
    assert(crc_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t fnv1_64_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_fnv1_64(*ptr, strlen(*ptr));
    assert(fnv1_64_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t fnv1a_64_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_fnv1a_64(*ptr, strlen(*ptr));
    assert(fnv1a_64_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t fnv1_32_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;


  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_fnv1_32(*ptr, strlen(*ptr));
    assert(fnv1_32_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t fnv1a_32_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_fnv1a_32(*ptr, strlen(*ptr));
    assert(fnv1a_32_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t hsieh_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

#ifdef HAVE_HSIEH_HASH
    hash_val= hashkit_hsieh(*ptr, strlen(*ptr));
#else
    hash_val= 1;
#endif
    assert(hsieh_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t murmur_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;

  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_murmur(*ptr, strlen(*ptr));
    assert(murmur_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}

static test_return_t jenkins_run (hashkit_st *hashk __attribute__((unused)))
{
  uint32_t x;
  const char **ptr;


  for (ptr= list_to_hash, x= 0; *ptr; ptr++, x++)
  {
    uint32_t hash_val;

    hash_val= hashkit_jenkins(*ptr, strlen(*ptr));
    assert(jenkins_values[x] == hash_val);
  }

  return TEST_SUCCESS;
}




/**
  @brief now we list out the tests.
*/

test_st allocation[]= {
  {"init", 0, (test_callback_fn)init_test},
  {"create and free", 0, (test_callback_fn)allocation_test},
  {"clone", 0, (test_callback_fn)clone_test},
  {0, 0, 0}
};

test_st hash_tests[] ={
  {"md5", 0, (test_callback_fn)md5_run },
  {"crc", 0, (test_callback_fn)crc_run },
  {"fnv1_64", 0, (test_callback_fn)fnv1_64_run },
  {"fnv1a_64", 0, (test_callback_fn)fnv1a_64_run },
  {"fnv1_32", 0, (test_callback_fn)fnv1_32_run },
  {"fnv1a_32", 0, (test_callback_fn)fnv1a_32_run },
  {"hsieh", 0, (test_callback_fn)hsieh_run },
  {"murmur", 0, (test_callback_fn)murmur_run },
  {"jenkis", 0, (test_callback_fn)jenkins_run },
  {0, 0, (test_callback_fn)0}
};

/*
 * The following test suite is used to verify that we don't introduce
 * regression bugs. If you want more information about the bug / test,
 * you should look in the bug report at
 *   http://bugs.launchpad.net/libmemcached
 */
test_st regression[]= {
  {0, 0, 0}
};

collection_st collection[] ={
  {"allocation", 0, 0, allocation},
  {"regression", 0, 0, regression},
  {"hashing", 0, 0, hash_tests},
  {0, 0, 0, 0}
};

/* Prototypes for functions we will pass to test framework */
void *world_create(void);
test_return_t world_destroy(hashkit_st *hashk);

void *world_create(void)
{
  hashkit_st *hashk_ptr;

  hashk_ptr= hashkit_create(&global_hashk);

  assert(hashk_ptr == &global_hashk);

  // First we test if hashk is even valid
  assert(hashkit_is_initialized(hashk_ptr) == true);
  assert(hashkit_is_allocated(hashk_ptr) == false);
  assert(hashk_ptr->continuum == NULL);

  return hashk_ptr;
}


test_return_t world_destroy(hashkit_st *hashk)
{
  // Did we get back what we expected?
  assert(hashkit_is_initialized(hashk) == true);
  assert(hashkit_is_allocated(hashk) == false);
  hashkit_free(&global_hashk);

  return TEST_SUCCESS;
}

void get_world(world_st *world)
{
  world->collections= collection;
  world->create= (test_callback_create_fn)world_create;
  world->destroy= (test_callback_fn)world_destroy;
}
