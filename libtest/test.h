/* uTest
 * Copyright (C) 2011 Data Differential, http://datadifferential.com/
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/*
  Structures for generic tests.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <libtest/visibility.h>

#pragma once

enum test_return_t {
  TEST_SUCCESS= 0, /* Backwards compatibility */
  TEST_FAILURE,
  TEST_MEMORY_ALLOCATION_FAILURE,
  TEST_SKIPPED,
  TEST_FATAL, // Collection should not be continued
  TEST_MAXIMUM_RETURN /* Always add new error code before */
};

#ifdef	__cplusplus
extern "C" {
#endif

typedef void* (test_callback_create_fn)(enum test_return_t*);
typedef enum test_return_t (test_callback_fn)(void *);
typedef enum test_return_t (test_callback_runner_fn)(test_callback_fn*, void *);
typedef enum test_return_t (test_callback_error_fn)(const enum test_return_t, void *);

#ifdef	__cplusplus
}
#endif

/**
  A structure describing the test case.
*/
struct test_st {
  const char *name;
  bool requires_flush;
  test_callback_fn *test_fn;
};


/**
  A structure which describes a collection of test cases.
*/
struct collection_st {
  const char *name;
  test_callback_fn *pre;
  test_callback_fn *post;
  struct test_st *tests;
};


/**
  Structure which houses the actual callers for the test cases contained in
  the collections.
*/
struct world_runner_st {
  test_callback_runner_fn *pre;
  test_callback_runner_fn *run;
  test_callback_runner_fn *post;
};


/**
  world_st is the structure which is passed to the test implementation to be filled.
  This must be implemented in order for the test framework to load the tests. We call
  get_world() in order to fill this structure.
*/

struct world_st {
  collection_st *collections;

  /* These methods are called outside of any collection call. */
  test_callback_create_fn *create;
  test_callback_fn *destroy;

  /* This is called a the beginning of any collection run. */
  test_callback_fn *collection_startup;

  /* This is called a the end of any collection run. */
  test_callback_fn *collection_shutdown;

  /* This is called a the beginning of any run. */
  test_callback_fn *run_startup;

  /* This called on a test if the test requires a flush call (the bool is from test_st) */
  test_callback_fn *flush;

  /**
    These are run before/after the test. If implemented. Their execution is not controlled
    by the test.
  */
  test_callback_fn *pre_run;
  test_callback_fn *post_run;

  /**
    If an error occurs during the test, this is called.
  */
  test_callback_error_fn *on_error;

  /**
    Runner represents the callers for the tests. If not implemented we will use
    a set of default implementations.
  */
  world_runner_st *runner;

  world_st() :
    collections(NULL),
    create(NULL),
    destroy(NULL),
    collection_startup(NULL),
    collection_shutdown(NULL),
    run_startup(NULL),
    flush(NULL),
    pre_run(NULL),
    post_run(NULL),
    on_error(NULL),
    runner(NULL)
  { }

  virtual ~world_st()
  { }

private:
  world_st(const world_st&);
  world_st& operator=(const world_st&);
};



/**
  @note world_stats_st is a simple structure for tracking test successes.
*/
struct world_stats_st {
  int32_t collection_success;
  int32_t collection_skipped;
  int32_t collection_failed;
  int32_t collection_total;

  uint32_t success;
  uint32_t skipped;
  uint32_t failed;
  uint32_t total;

  world_stats_st() :
    collection_success(0),
    collection_skipped(0),
    collection_failed(0),
    collection_total(0),
    success(0),
    skipped(0),
    failed(0),
    total(0)
  { }
};

#define TEST_STRINGIFY(x) #x
#define TEST_TOSTRING(x) TEST_STRINGIFY(x)
#define TEST_AT __FILE__ ":" TEST_TOSTRING(__LINE__)

#ifdef	__cplusplus
extern "C" {
#endif

/* How we make all of this work :) */
LIBTEST_API
void get_world(world_st *world);

LIBTEST_INTERNAL_API
void create_core(void);

/**
  @note Friendly print function for errors.
*/
LIBTEST_INTERNAL_API
const char *test_strerror(test_return_t code);

#define test_assert_errno(A) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: ", __FILE__, __LINE__);\
    perror(#A); \
    fprintf(stderr, "\n"); \
    create_core(); \
    assert((A)); \
  } \
} while (0)

#define test_truth(A) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: %s\n", __FILE__, __LINE__, #A);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_true(A) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: %s\n", __FILE__, __LINE__, #A);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_true_got(A,B) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: \"%s\" received \"%s\"\n", __FILE__, __LINE__, #A, (B));\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_skip(A,B) \
do \
{ \
  if ((A) != (B)) \
  { \
    return TEST_SKIPPED; \
  } \
} while (0)

#define test_fail(A) \
do \
{ \
  if (1) { \
    fprintf(stderr, "\nFailed at %s:%d: %s\n", __FILE__, __LINE__, #A);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)


#define test_false(A) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\nAssertion failed in %s:%d: %s\n", __FILE__, __LINE__, #A);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_false_with(A,B) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: %s with %s\n", __FILE__, __LINE__, #A, (B));\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)


#define test_compare(A,B) \
do \
{ \
  if ((A) != (B)) \
  { \
    fprintf(stderr, "\n%s:%d: Expected %s, got %lu\n", __FILE__, __LINE__, #A, (unsigned long)(B)); \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_compare_got(A,B,C) \
do \
{ \
  if ((A) != (B)) \
  { \
    fprintf(stderr, "\n%s:%d: Expected %s, got %s\n", __FILE__, __LINE__, #A, (C)); \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)


#define test_strcmp(A,B) \
do \
{ \
  if (strcmp((A), (B))) \
  { \
    fprintf(stderr, "\n%s:%d: Expected %s, got %s\n", __FILE__, __LINE__, (A), (B)); \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_memcmp(A,B,C) \
do \
{ \
  if (memcmp((A), (B), (C))) \
  { \
    fprintf(stderr, "\n%s:%d: %.*s -> %.*s\n", __FILE__, __LINE__, (int)(C), (char *)(A), (int)(C), (char *)(B)); \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#ifdef	__cplusplus
}
#endif

#ifdef __cplusplus
#define test_literal_param(X) (X), (static_cast<size_t>((sizeof(X) - 1)))
#else
#define test_literal_param(X) (X), ((size_t)((sizeof(X) - 1)))
#endif

#define test_string_make_from_cstr(X) (X), ((X) ? strlen(X) : 0)
