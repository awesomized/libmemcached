/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  uTest, libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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


#include <libtest/common.h>
#include <iostream>

using namespace libtest;

static test_return_t _default_callback(void *p)
{
  (void)p;

  return TEST_SUCCESS;
}

static Runner defualt_runners;

Framework::Framework() :
  collections(NULL),
  _create(NULL),
  _destroy(NULL),
  collection_startup(_default_callback),
  collection_shutdown(_default_callback),
  _on_error(NULL),
  _runner(NULL),
  _creators_ptr(NULL)
{
}

Framework::~Framework()
{
  if (_destroy and _destroy(_creators_ptr))
  {
    Error << "Failure in _destroy(), some resources may not have been cleaned up.";
  }

  _servers.shutdown();
}

test_return_t Framework::Item::pre(void *arg)
{
  if (pre_run)
  {
    return pre_run(arg);
  }

  return TEST_SUCCESS;
}

test_return_t Framework::Item::post(void *arg)
{
  if (post_run)
  {
    return post_run(arg);
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

libtest::Runner *Framework::runner()
{
  return _runner ? _runner : &defualt_runners;
}

void* Framework::create(test_return_t& arg)
{
  arg= TEST_SUCCESS;
  if (_create)
  {
    return _creators_ptr= _create(_servers, arg);
  }

  return NULL;
}
