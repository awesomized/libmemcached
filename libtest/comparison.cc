/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
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

namespace libtest {

bool valgrind_is_caller(void)
{
  if (bool(getenv("TESTS_ENVIRONMENT")) and strstr(getenv("TESTS_ENVIRONMENT"), "valgrind"))
  {
    return true;
  }

  return false;
}

bool gdb_is_caller(void)
{
  if (bool(getenv("TESTS_ENVIRONMENT")) and strstr(getenv("TESTS_ENVIRONMENT"), "gdb"))
  {
    return true;
  }

  return false;
}

bool helgrind_is_caller(void)
{
  if (bool(getenv("TESTS_ENVIRONMENT")) and strstr(getenv("TESTS_ENVIRONMENT"), "helgrind"))
  {
    return true;
  }

  return false;
}

bool _in_valgrind(const char*, int, const char*)
{
  if (valgrind_is_caller())
  {
    return true;
  }

  return false;
}

} // namespace libtest
