/* 
 * uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#pragma once

/**
  A structure describing the test case.
*/
struct test_st {
  const char *name;
  bool requires_flush;
  test_callback_fn *test_fn;
};

#define TEST_STRINGIFY(x) #x
#define TEST_TOSTRING(x) TEST_STRINGIFY(x)
#define TEST_AT __FILE__ ":" TEST_TOSTRING(__LINE__)


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

#define test_assert(A, B) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\nAssertion, %s(%s), failed at %s:%d: ", (B), #A, __FILE__, __LINE__);\
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

