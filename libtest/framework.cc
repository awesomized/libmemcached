/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include <libtest/common.h>

test_return_t Framework::destroy(void* arg)
{
  if (_destroy)
  {
    return _destroy(arg);
  }

  return TEST_SUCCESS;
}

test_return_t Framework::Item::flush(void* arg, test_st* run)
{
  if (run->requires_flush and _flush)
  {
    return _flush(arg);
  }

  return TEST_SUCCESS;
}

test_return_t Framework::on_error(const test_return_t rc, void* arg)
{
  if (_on_error and test_failed(_on_error(rc, arg)))
  {
    return TEST_FAILURE;
  }

  return TEST_SUCCESS;
}

test_return_t Framework::startup(void* arg)
{
  if (collection_startup)
  {
    return collection_startup(arg);
  }

  return TEST_SUCCESS;
}

test_return_t Framework::Item::startup(void* arg)
{
  if (_startup)
  {
    return _startup(arg);
  }

  return TEST_SUCCESS;
}
