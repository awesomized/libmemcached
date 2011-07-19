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

#include <iostream>
#include <cassert>
#include <sstream>
#include <ctime>
#include <ostream>

namespace libtest {
namespace stream {

namespace detail {

template<class Ch, class Tr, class A>
  class cerr {
  private:

  public:
    typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;

  public:
    void operator()(const stream_buffer &s)
    {
      std::cerr << s.str() << std::endl;
    }
  };

template<class Ch, class Tr, class A>
  class make_cerr {
  private:

  public:
    typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;

  public:
    void operator()(const stream_buffer &s)
    {
      std::cerr << std::endl << s.str() << std::endl;
    }
  };

template<class Ch, class Tr, class A>
  class cout {
  private:

  public:
    typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;

  public:
    void operator()(const stream_buffer &s)
    {
      std::cout << s.str() << std::endl;
    }
  };

template<class Ch, class Tr, class A>
  class clog {
  private:

  public:
    typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;

  public:
    void operator()(const stream_buffer &s)
    {
      std::cerr<< s.str() << std::endl;
    }
  };

template<template <class Ch, class Tr, class A> class OutputPolicy, class Ch = char, class Tr = std::char_traits<Ch>, class A = std::allocator<Ch> >
  class log {
  private:
    typedef OutputPolicy<Ch, Tr, A> output_policy;
    const char *_filename;
    int _line_number;
    const char *_func;

  public:
    log() :
      _filename(NULL),
      _line_number(0)
    { }

    void set_filename(const char *filename, int line_number, const char *func)
    {
      _filename= filename;
      _line_number= line_number;
      _func= func;
    }

    ~log()
    {
      output_policy()(arg);
    }

  public:
    template<class T>
      log &operator<<(const T &x)
      {
        if (_filename)
        {
          arg << _filename << ":" << _line_number << ": in " << _func << "() ";
          _filename= NULL;
        }
        arg << x;
        return *this;
      }

  private:
    typename output_policy::stream_buffer arg;
  };
}

class make_cerr : public detail::log<detail::make_cerr> {
public:
  make_cerr(const char *filename, int line_number, const char *func)
  {
    set_filename(filename, line_number, func);
  }
};

class cerr : public detail::log<detail::cerr> {
public:
  cerr(const char *filename, int line_number, const char *func)
  {
    set_filename(filename, line_number, func);
  }
};

class clog : public detail::log<detail::clog> {
public:
  clog(const char *, int, const char*)
  { }
};

class cout : public detail::log<detail::cout> {
public:
  cout(const char *, int, const char *)
  { }
};


} // namespace stream

#define Error stream::cerr(__FILE__, __LINE__, __func__)

#define Out stream::cout(NULL, __LINE__, __func__)

#define Outn() stream::cout(NULL, __LINE__, __func__) << " "

#define Log stream::clog(NULL, __LINE__, __func__)

#define Logn() stream::clog(NULL, __LINE__, __func__) << " "

} // namespace libtest
