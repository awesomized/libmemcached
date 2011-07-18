/* 
 * uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <libtest/stream.h>
#include <libtest/comparison.hpp>

/**
  A structure describing the test case.
*/
struct test_st {
  const char *name;
  bool requires_flush;
  test_callback_fn *test_fn;
};

#define test_assert_errno(A) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\n%s:%d: Assertion failed for %s: ", __FILE__, __LINE__, __func__);\
    perror(#A); \
    fprintf(stderr, "\n"); \
    create_core(); \
    assert((A)); \
  } \
} while (0)

#define test_assert(A, B) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\n%s:%d: Assertion failed %s, with message %s, in %s", __FILE__, __LINE__, (B), #A, __func__ );\
    fprintf(stderr, "\n"); \
    create_core(); \
    assert((A)); \
  } \
} while (0)

#define test_truth(A) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\n%s:%d: Assertion \"%s\" failed, in %s\n", __FILE__, __LINE__, #A, __func__);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_true(A) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\n%s:%d: Assertion \"%s\" failed, in %s\n", __FILE__, __LINE__, #A, __func__);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_true_got(__expected, __hint) \
do \
{ \
  if (not libtest::_compare_true_hint(__FILE__, __LINE__, __func__, ((__expected)), #__expected, ((__hint)))) \
  { \
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
    fprintf(stderr, "\n%s:%d: Failed with %s, in %s\n", __FILE__, __LINE__, #A, __func__);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)


#define test_false(A) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\n%s:%d: Assertion failed %s, in %s\n", __FILE__, __LINE__, #A, __func__);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_false_with(A,B) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\n%s:%d: Assertion failed %s with %s\n", __FILE__, __LINE__, #A, (B));\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_compare(__expected, __actual) \
do \
{ \
  if (not libtest::_compare(__FILE__, __LINE__, __func__, ((__expected)), ((__actual)))) \
  { \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_zero(__actual) \
do \
{ \
  if (not libtest::_compare_zero(__FILE__, __LINE__, __func__, ((__actual)))) \
  { \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_compare_got(__expected, __actual, __hint) \
do \
{ \
  if (not libtest::_compare_hint(__FILE__, __LINE__, __func__, (__expected), (__actual), (__hint))) \
  { \
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

#define test_return_if(__test_return_t) \
do \
{ \
  if ((__test_return_t) != TEST_SUCCESS) \
  { \
    return __test_return_t; \
  } \
} while (0)

