/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Data Differential YATL (i.e. libtest)  library
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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

#include <libtest/fatal.hpp>

namespace libtest {

class __test_result : public std::exception
{
public:
  __test_result(const char *file, int line, const char *func);

  int line()
  {
    return _line;
  }

  const char*  file()
  {
    return _file;
  }

  const char* func()
  {
    return _func;
  }

private:
  int _line;
  const char*  _file;
  const char* _func;
};

class __success : public __test_result
{
public:
  __success(const char *file, int line, const char *func);

  const char* what() const throw()
  {
    return "SUCCESS";
  }

private:
};

class __skipped : public __test_result
{
public:
  __skipped(const char *file, int line, const char *func);

  const char* what() const throw()
  {
    return "SKIPPED";
  }

private:
};

class __failure : public __test_result
{
public:
  __failure(const char *file, int line, const char *func, const std::string&);

  const char* what() const throw()
  {
    return _error_message;
  }

private:
  char _error_message[BUFSIZ];
};


} // namespace libtest

#define _SUCCESS throw libtest::__success(LIBYATL_DEFAULT_PARAM)
#define SKIP throw libtest::__skipped(LIBYATL_DEFAULT_PARAM)
#define FAIL(__mesg) throw libtest::__failure(LIBYATL_DEFAULT_PARAM, __mesg)
