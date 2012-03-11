/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <config.h>
#include <libtest/common.h>

#include <libtest/failed.h>

#include <iostream>
#include <string>
#include <vector>

namespace libtest {

struct failed_st {
  failed_st(const std::string collection_arg, const std::string test_arg) :
    collection(collection_arg),
    test(test_arg)
  { }

  std::string collection;
  std::string test;
};

typedef std::vector<failed_st> Failures;

class Failed
{
private:
  Failures failures;

public:
  void push(const char *collection, const char *test)
  {
    failures.push_back(failed_st(collection, test));
  }

  void print_failed_test(void)
  {
    for (Failures::iterator iter= failures.begin(); iter != failures.end(); iter++)
    {
      Error << "\t" << (*iter).collection << " " << (*iter).test;
    }
  }
};

} // namespace libtest
