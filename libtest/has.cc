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

#include <config.h>
#include <libtest/common.h>

#include <cstdlib>
#include <unistd.h>

namespace libtest {

bool has_libmemcached(void)
{
  if (HAVE_LIBMEMCACHED)
  {
    return true;
  }

  return false;
}

bool has_libdrizzle(void)
{
  if (HAVE_LIBDRIZZLE)
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


bool has_gearmand()
{
  if (HAVE_GEARMAND_BINARY)
  {
    std::stringstream arg_buffer;

    if (getenv("PWD") and strcmp(MEMCACHED_BINARY, "gearmand/gearmand") == 0)
    {
      arg_buffer << getenv("PWD");
      arg_buffer << "/";
    }
    arg_buffer << GEARMAND_BINARY;

    if (access(arg_buffer.str().c_str(), X_OK) == 0)
    {
      return true;
    }
  }

  return false;
}

bool has_drizzled()
{
  if (HAVE_DRIZZLED_BINARY)
  {
    if (access(DRIZZLED_BINARY, X_OK) == 0)
    {
      return true;
    }
  }

  return false;
}

bool has_mysqld()
{
  if (HAVE_MYSQLD_BUILD)
  {
    if (access(MYSQLD_BINARY, X_OK) == 0)
    {
      return true;
    }
  }

  return false;
}

bool has_memcached()
{
  if (HAVE_MEMCACHED_BINARY)
  {
    std::stringstream arg_buffer;

    if (getenv("PWD") and strcmp(MEMCACHED_BINARY, "memcached/memcached") == 0)
    {
      arg_buffer << getenv("PWD");
      arg_buffer << "/";
    }
    arg_buffer << MEMCACHED_BINARY;

    if (access(arg_buffer.str().c_str(), X_OK) == 0)
    {
      return true;
    }
  }

  return false;
}

bool has_memcached_sasl()
{
#if defined(HAVE_MEMCACHED_SASL_BINARY) && HAVE_MEMCACHED_SASL_BINARY
  if (HAVE_MEMCACHED_SASL_BINARY)
  {
    if (access(MEMCACHED_SASL_BINARY, X_OK) == 0)
    {
      return true;
    }
  }
#endif

  return false;
}

} // namespace libtest
