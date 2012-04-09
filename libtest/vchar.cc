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

namespace libtest {

static std::string printer(const char *str, size_t length)
{
  std::ostringstream buf;
  for (size_t x= 0; x < length; x++)
  {
    if (isprint(str[x]))
    {
      buf << str[x];
    }
    else
    {
      buf << "(" << int(str[x]) << ")";
    }
  }

  return buf.str();
}

namespace vchar {

int compare(libtest::vchar_t& arg, const char *str, size_t length)
{
  if (arg.size() == length and (memcmp(&arg[0], str, length) == 0))
  {
    return 0;
  }
  else if (arg.size() > length)
  {
    return 1;
  }

  return -1;
}

void make(libtest::vchar_t& arg)
{
  size_t length= rand() % 1024;
  make(arg, length);
}

void make(libtest::vchar_t& arg, size_t length)
{
  for (uint32_t x= 0; x < length; x++)
  {
    arg.push_back(char(x % 127));
  }
}

} // namespace vchar

void make_vector(libtest::vchar_t& arg, const char *str, size_t length)
{
  arg.resize(length);
  memcpy(&arg[0], str, length);
}

std::ostream& operator<<(std::ostream& output, const libtest::vchar_t& arg)
{
  std::string tmp= libtest::printer(&arg[0], arg.size());
  output << tmp <<  "[" << arg.size() << "]";

  return output;
}

} // namespace libtest
