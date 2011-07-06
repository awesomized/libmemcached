/* uTest Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include <libtest/common.h>
#include <iostream>

static test_return_t _runner_default(test_callback_fn func, void *p)
{
  if (func)
  {
    return func(p);
  }

  return TEST_SUCCESS;
}

static Runner defualt_runners= {
  _runner_default,
  _runner_default,
  _runner_default
};

static test_return_t _default_callback(void *p)
{
  (void)p;

  return TEST_SUCCESS;
}

Framework::Framework() :
  collections(NULL),
  _create(NULL),
  _destroy(NULL),
  collection_startup(_default_callback),
  collection_shutdown(_default_callback),
  _on_error(NULL),
  runner(&defualt_runners),
  _creators_ptr(NULL)
{
}

Framework::~Framework()
{
  if (_destroy)
  {
    if (test_failed(_destroy(_creators_ptr)))
    {
      std::cerr << "Failure in _destroy(), some resources may not have been cleaned up." << std::endl;
    }
  }
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

void* Framework::create(test_return_t& arg)
{
  arg= TEST_SUCCESS;
  if (_create)
  {
    return _creators_ptr= _create(&arg);
  }

  return NULL;
}
