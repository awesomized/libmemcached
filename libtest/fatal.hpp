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

#pragma once

#include <stdexcept>

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __func__
#endif

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

  // The following are just for unittesting the exception class
  static bool is_disabled();
  static bool disable();
  static bool enable();
  static uint32_t disabled_counter();
  static void increment_disabled_counter();

private:
  char _error_message[BUFSIZ];
};


} // namespace libtest

#define fatal_message(__mesg) libtest::fatal(LIBYATL_DEFAULT_PARAM, "%s", __mesg)
#define fatal_assert(__assert) if((__assert)) {} else { libtest::fatal(LIBYATL_DEFAULT_PARAM, "%s", #__assert); }
