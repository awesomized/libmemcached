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
#include <string>

char _libtool[1024]= { 0 };

namespace libtest {

const char *libtool(void)
{
  if (_libtool[0] == 0)
  {
    std::string libtool_buffer;
    if (getenv("PWD"))
    {
      libtool_buffer+= getenv("PWD");
      libtool_buffer+= "/";
    }
    else
    {
      libtool_buffer+= "./";
    }

    libtool_buffer+= "libtool";
    if (access(libtool_buffer.c_str(), R_OK | W_OK | X_OK))
    {
      Error << "Could not find libtool via access(" << libtool_buffer << ") :" << strerror(errno);
      return NULL;
    }

    snprintf(_libtool, sizeof(_libtool), "%s", libtool_buffer.c_str());
  }

  return _libtool;
}

} // namespace libtest
