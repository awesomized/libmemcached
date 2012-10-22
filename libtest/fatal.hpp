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

#include <stdexcept>

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

#define YATL_STRINGIFY(x) #x
#define YATL_TOSTRING(x) YATL_STRINGIFY(x)
#define YATL_AT __FILE__ ":" YATL_TOSTRING(__LINE__)
#define YATL_AT_PARAM __func__, AT
#define YATL_UNIQUE __FILE__ ":" YATL_TOSTRING(__LINE__) "_unique"
#define YATL_UNIQUE_FUNC_NAME __FILE__ ":" YATL_TOSTRING(__LINE__) "_unique_func"

#define LIBYATL_DEFAULT_PARAM __FILE__, __LINE__, __PRETTY_FUNCTION__

namespace libtest {

class fatal : std::runtime_error
{
public:
  fatal(const char *file, int line, const char *func, const char *format, ...);

  const char* what() const throw()
  {
    return _error_message;
  }

  const char* mesg() const throw()
  {
    return _error_message;
  }

  // The following are just for unittesting the exception class
  static bool is_disabled();
  static void disable();
  static void enable();
  static uint32_t disabled_counter();
  static void increment_disabled_counter();

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
  char _error_message[BUFSIZ];
  char _mesg[BUFSIZ];
  int _line;
  const char*  _file;
  const char* _func;
};

class disconnected : std::runtime_error
{
public:
  disconnected(const char *file, int line, const char *func, const std::string&, const in_port_t port, const char *format, ...);

  const char* what() const throw()
  {
    return _error_message;
  }

  // The following are just for unittesting the exception class
  static bool is_disabled();
  static void disable();
  static void enable();
  static uint32_t disabled_counter();
  static void increment_disabled_counter();

  int line()
  {
    return _line;
  }

  const char* file()
  {
    return _file;
  }

  const char* func()
  {
    return _func;
  }

private:
  char _error_message[BUFSIZ];
  in_port_t _port;
  char _instance[1024];
  int _line;
  const char*  _file;
  const char* _func;
};


} // namespace libtest

#define fatal_message(__mesg) throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "%s", __mesg)
#define fatal_assert(__assert) if((__assert)) {} else { throw libtest::fatal(LIBYATL_DEFAULT_PARAM, "%s", #__assert); }
