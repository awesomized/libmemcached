/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2010 Brian Aker
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include <libtest/visibility.h>

/*
  Structures for generic tests.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if !defined(__cplusplus)
# include <stdbool.h>
#endif

typedef struct world_st world_st;
typedef struct collection_st collection_st;
typedef struct test_st test_st;

typedef enum {
  TEST_SUCCESS= 0, /* Backwards compatibility */
  TEST_FAILURE,
  TEST_MEMORY_ALLOCATION_FAILURE,
  TEST_SKIPPED,
  TEST_MAXIMUM_RETURN /* Always add new error code before */
} test_return_t;

typedef void *(*test_callback_create_fn)(test_return_t *error);
typedef test_return_t (*test_callback_fn)(void *);
typedef test_return_t (*test_callback_runner_fn)(test_callback_fn, void *);
typedef test_return_t (*test_callback_error_fn)(test_return_t, void *);

/**
  A structure describing the test case.
*/
struct test_st {
  const char *name;
  bool requires_flush;
  test_callback_fn test_fn;
};


/**
  A structure which describes a collection of test cases.
*/
struct collection_st {
  const char *name;
  test_callback_fn pre;
  test_callback_fn post;
  test_st *tests;
};


/**
  Structure which houses the actual callers for the test cases contained in
  the collections.
*/
typedef struct {
  test_callback_runner_fn pre;
  test_callback_runner_fn run;
  test_callback_runner_fn post;
} world_runner_st;


/**
  world_st is the structure which is passed to the test implementation to be filled.
  This must be implemented in order for the test framework to load the tests. We call
  get_world() in order to fill this structure.
*/

struct world_st {
  collection_st *collections;

  /* These methods are called outside of any collection call. */
  test_callback_create_fn create;
  test_callback_fn destroy;

  struct {
    /* This is called a the beginning of any test run. */
    test_callback_fn startup;

    /* This called on a test if the test requires a flush call (the bool is from test_st) */
    test_callback_fn flush;

    /**
      These are run before/after the test. If implemented. Their execution is not controlled
      by the test.
    */
    test_callback_fn pre_run;
    test_callback_fn post_run;

    /**
      If an error occurs during the test, this is called.
    */
    test_callback_error_fn on_error;
  } test;

  struct {
    /* This is called a the beginning of any collection run. */
    test_callback_fn startup;

    /* This is called at the end of any collection run. */
    test_callback_fn shutdown;
  } collection;


  /**
    Runner represents the callers for the tests. If not implemented we will use
    a set of default implementations.
  */
  world_runner_st *runner;
};



/**
  @note world_stats_st is a simple structure for tracking test successes.
*/
typedef struct {
  uint32_t collection_success;
  uint32_t collection_skipped;
  uint32_t collection_failed;
  uint32_t collection_total;
  uint32_t success;
  uint32_t skipped;
  uint32_t failed;
  uint32_t total;
} world_stats_st;

#ifdef	__cplusplus
extern "C" {
#endif

/* Help function for use with gettimeofday() */
LIBTEST_API
long int timedif(struct timeval a, struct timeval b);

/* How we make all of this work :) */
LIBTEST_API
void get_world(world_st *world);

LIBTEST_INTERNAL_API
void create_core(void);

/**
  @note Friendly print function for errors.
*/
LIBTEST_API
const char *test_strerror(test_return_t code);

#define test_fail(A) \
do \
{ \
  if (1) { \
    fprintf(stderr, "\nFailed at %s:%d: %s\n", __FILE__, __LINE__, #A);\
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

#define test_false(A) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: %s\n", __FILE__, __LINE__, #A);\
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

#define test_strcmp(A,B) \
do \
{ \
  if (strcmp((A), (B))) \
  { \
    fprintf(stderr, "\n%s:%d: `%s` -> `%s`\n", __FILE__, __LINE__, (A), (B)); \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#ifdef __cplusplus
#define STRING_WITH_LEN(X) (X), (static_cast<size_t>((sizeof(X) - 1)))
#else
#define STRING_WITH_LEN(X) (X), ((size_t)((sizeof(X) - 1)))
#endif

#ifdef __cplusplus
#define STRING_PARAM_WITH_LEN(X) X, static_cast<size_t>(sizeof(X) - 1)
#else
#define STRING_PARAM_WITH_LEN(X) X, (size_t)((sizeof(X) - 1))
#endif

#ifdef	__cplusplus
}
#endif
