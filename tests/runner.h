/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached Client and Server 
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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

class LibmemcachedRunner : public libtest::Runner {
public:
  test_return_t run(test_callback_fn* func, void *object)
  {
    return _runner_default(libmemcached_test_callback_fn(func), (libmemcached_test_container_st*)object);
  }

  test_return_t pre(test_callback_fn* func, void *object)
  {
    return _pre_runner_default(libmemcached_test_callback_fn(func), (libmemcached_test_container_st*)object);
  }

  test_return_t post(test_callback_fn* func, void *object)
  {
    return _post_runner_default(libmemcached_test_callback_fn(func), (libmemcached_test_container_st*)object);
  }

private:
  test_return_t _runner_default(libmemcached_test_callback_fn func, libmemcached_test_container_st *container)
  {
    test_compare(true, check());

    if (func)
    {
      test_true(container);
      test_true(container->memc);
      test_return_t ret;
      try {
        ret= func(container->memc);
      }
      catch (std::exception& e)
      {
        libtest::Error << e.what();
        return TEST_FAILURE;
      }

      return ret;
    }

    return TEST_SUCCESS;
  }

  test_return_t _pre_runner_default(libmemcached_test_callback_fn func, libmemcached_test_container_st *container)
  {
    if (func)
    {
      return func(container->parent);
    }

    return TEST_SUCCESS;
  }

  test_return_t _post_runner_default(libmemcached_test_callback_fn func, libmemcached_test_container_st *container)
  {
    if (func)
    {
      return func(container->parent);
    }

    return TEST_SUCCESS;
  }
};

