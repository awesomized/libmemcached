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
#include <libtest/has.hpp>

#include <cstdlib>

bool has_memcached_support(void)
{
  if (HAVE_LIBMEMCACHED and HAVE_MEMCACHED_BINARY)
  {
    return true;
  }

  return false;
}

bool has_drizzle_support(void)
{
  if (HAVE_LIBDRIZZLE and HAVE_DRIZZLED_BINARY)
  {
    return true;
  }

  return false;
}

bool has_postgres_support(void)
{
  if (getenv("POSTGES_IS_RUNNING_AND_SETUP"))
  {

    if (HAVE_LIBPQ)
    {
      return true;
    }
  }

  return false;
}
