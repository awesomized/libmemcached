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


#pragma once

/**
  Framework is the structure which is passed to the test implementation to be filled.
  This must be implemented in order for the test framework to load the tests. We call
  get_world() in order to fill this structure.
*/

struct Framework {
  collection_st *collections;

  /* These methods are called outside of any collection call. */
  test_callback_create_fn *_create;
  test_callback_fn *_destroy;

  void* create(test_return_t& arg);

  /* This is called a the beginning of any collection run. */
  test_callback_fn *collection_startup;

  test_return_t startup(void*);

  /* This is called a the end of any collection run. */
  test_callback_fn *collection_shutdown;

  test_return_t shutdown(void* arg)
  {
    if (collection_shutdown)
    {
      return collection_shutdown(arg);
    }

    return TEST_SUCCESS;
  }

  /**
    These are run before/after the test. If implemented. Their execution is not controlled
    by the test.
  */
  struct Item {
    /* This is called a the beginning of any run. */
    test_callback_fn *_startup;

    test_return_t startup(void*);

    /* 
      This called on a test if the test requires a flush call (the bool is
      from test_st) 
    */
    test_callback_fn *_flush;

    /*
       Run before and after the runnner is executed.
    */
    test_callback_fn *pre_run;
    test_callback_fn *post_run;

    Item() :
      _startup(NULL),
      _flush(NULL),
      pre_run(NULL),
      post_run(NULL)
    { }

    test_return_t flush(void* arg, test_st* run);

    void set_pre(test_callback_fn *arg)
    {
      pre_run= arg;
    }

    void set_post(test_callback_fn *arg)
    {
      pre_run= arg;
    }

    test_return_t pre(void *arg)
    {
      if (pre_run)
      {
        return pre_run(arg);
      }

      return TEST_SUCCESS;
    }

    test_return_t post(void *arg)
    {
      if (post_run)
      {
        return post_run(arg);
      }

      return TEST_SUCCESS;
    }

  } item;

  /**
    If an error occurs during the test, this is called.
  */
  test_callback_error_fn *_on_error;

  test_return_t on_error(const enum test_return_t, void *);

  /**
    Runner represents the callers for the tests. If not implemented we will use
    a set of default implementations.
  */
  Runner *runner;

  Framework();

  virtual ~Framework();

  Framework(const Framework&);

private:
  Framework& operator=(const Framework&);
  void *_creators_ptr;
};
