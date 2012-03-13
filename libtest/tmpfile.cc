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


#include <libtest/common.h>

namespace libtest {

std::string create_tmpfile(const std::string& name)
{
  char file_buffer[FILENAME_MAX];
  file_buffer[0]= 0;

  int length= snprintf(file_buffer, sizeof(file_buffer), "var/tmp/%s.XXXXXX", name.c_str());
  fatal_assert(length > 0);

  int fd;
  if ((fd= mkstemp(file_buffer)) == -1)
  {
    libtest::fatal(LIBYATL_DEFAULT_PARAM, "mkstemp() failed on %s with %s", file_buffer, strerror(errno));
  }
  close(fd);
  unlink(file_buffer);

  return file_buffer;
}

} // namespace libtest
