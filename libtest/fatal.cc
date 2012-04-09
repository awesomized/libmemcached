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
#include <cstdarg>

namespace libtest {

fatal::fatal(const char *file, int line, const char *func, const char *format, ...) :
  std::runtime_error(func)
  {
    va_list args;
    va_start(args, format);
    char last_error[BUFSIZ];
    (void)vsnprintf(last_error, sizeof(last_error), format, args);
    va_end(args);

    snprintf(_error_message, sizeof(_error_message), "%s:%d FATAL:%s (%s)", file, int(line), last_error, func);
  }

static bool _disabled= false;
static uint32_t _counter= 0;

bool fatal::is_disabled()
{
  return _disabled;
}

void fatal::disable()
{
  _disabled= true;
}

void fatal::enable()
{
  _disabled= false;
}

uint32_t fatal::disabled_counter()
{
  return _counter;
}

void fatal::increment_disabled_counter()
{
  _counter++;
}

} // namespace libtest

