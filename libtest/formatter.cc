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

#include "libtest/yatlcon.h"

#include <libtest/common.h>

#include <iostream>
#include <fstream>
  
namespace libtest {

class TestCase {
public:
  TestCase(const std::string& arg):
    _name(arg),
    _result(TEST_FAILURE)
  {
  }

  const std::string& name() const
  {
    return _name;
  }

  test_return_t result() const
  {
    return _result;
  }

  void result(test_return_t arg)
  {
    _result= arg;
  }

  void result(test_return_t arg, const libtest::Timer& timer_)
  {
    _result= arg;
    _timer= timer_;
  }

  const libtest::Timer& timer() const
  {
    return _timer;
  }

  void timer(libtest::Timer& arg)
  {
    _timer= arg;
  }

private:
  std::string _name;
  test_return_t _result;
  libtest::Timer _timer;
};

Formatter::Formatter(const std::string& frame_name, const std::string& arg)
{
  _suite_name= frame_name;
  _suite_name+= ".";
  _suite_name+= arg;
}

Formatter::~Formatter()
{
  for (TestCases::iterator iter= _testcases.begin(); iter != _testcases.end(); ++iter)
  {
    delete *iter;
  }
}

TestCase* Formatter::current()
{
  return _testcases.back();
}

void Formatter::skipped()
{
  current()->result(TEST_SKIPPED);
  Out << name() << "." << current()->name() <<  "\t\t\t\t\t" << "[ " << test_strerror(current()->result()) << " ]";

  reset();
}

void Formatter::failed()
{
  assert(current());
  current()->result(TEST_FAILURE);

  Out << name() << "." << current()->name() <<  "\t\t\t\t\t" << "[ " << test_strerror(current()->result()) << " ]";

  reset();
}

void Formatter::success(const libtest::Timer& timer_)
{
  assert(current());
  current()->result(TEST_SUCCESS, timer_);

  Out << name() << "."
    << current()->name()
    <<  "\t\t\t\t\t" 
    << current()->timer() 
    << " [ " << test_strerror(current()->result()) << " ]";

  reset();
}

void Formatter::xml(libtest::Framework& framework_, std::ofstream& output)
{
  output << "<testsuites name=\"" << framework_.name() << "\">" << std::endl;
  for (Suites::iterator framework_iter= framework_.suites().begin();
       framework_iter != framework_.suites().end();
       ++framework_iter)
  {
    output << "\t<testsuite name=\"" << (*framework_iter)->name() << "\"  classname=\"\" package=\"\">" << std::endl;

    for (TestCases::iterator case_iter= (*framework_iter)->formatter()->testcases().begin();
         case_iter != (*framework_iter)->formatter()->testcases().end();
         ++case_iter)
    {
      output << "\t\t<testcase name=\"" 
        << (*case_iter)->name() 
        << "\" time=\"" 
        << (*case_iter)->timer().elapsed_milliseconds() 
        << "\">" 
        << std::endl;

      switch ((*case_iter)->result())
      {
        case TEST_SKIPPED:
        output << "\t\t <skipped/>" << std::endl;
        break;

        case TEST_FAILURE:
        output << "\t\t <failure message=\"\" type=\"\"/>"<< std::endl;
        break;

        case TEST_SUCCESS:
        break;
      }
      output << "\t\t</testcase>" << std::endl;
    }
    output << "\t</testsuite>" << std::endl;
  }
  output << "</testsuites>" << std::endl;
}

void Formatter::push_testcase(const std::string& arg)
{
  assert(_suite_name.empty() == false);
  TestCase* _current_testcase= new TestCase(arg);
  _testcases.push_back(_current_testcase);
}

void Formatter::reset()
{
}
} // namespace libtest
