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

#include <libtest/drizzled.h>

#include "util/instance.hpp"
#include "util/operation.hpp"

using namespace datadifferential;
using namespace libtest;

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#if defined(HAVE_LIBDRIZZLE) && HAVE_LIBDRIZZLE
#include <libdrizzle-1.0/drizzle_client.h>
#endif

using namespace libtest;

namespace libtest {
bool ping_drizzled(const in_port_t _port)
{
#if defined(HAVE_LIBDRIZZLE) && HAVE_LIBDRIZZLE
  {
    drizzle_st *drizzle= drizzle_create(NULL);

    if (drizzle == NULL)
    {
      return false;
    }

    drizzle_con_st *con;

    if ((con= drizzle_con_create(drizzle, NULL)) == NULL)
    {
      drizzle_free(drizzle);
      return false;
    }

    drizzle_con_set_tcp(con, "localhost", _port);
    drizzle_con_set_auth(con, "root", 0);

    bool success= false;

    drizzle_return_t rc;
    if ((rc= drizzle_con_connect(con)) == DRIZZLE_RETURN_OK)
    {
      drizzle_result_st *result= drizzle_ping(con, NULL, &rc);
      success= bool(result);
      drizzle_result_free(result);
    }

    if (success == true)
    { }
    else if (rc != DRIZZLE_RETURN_OK)
    {
      Error << drizzle_error(drizzle) << " localhost:" << _port;
    }

    drizzle_con_free(con);
    drizzle_free(drizzle);

    return success;
  }
#endif

  return false;
}
} // namespace libtest

class Drizzle : public libtest::Server
{
private:
public:
  Drizzle(const std::string& host_arg, in_port_t port_arg) :
    libtest::Server(host_arg, port_arg, DRIZZLED_BINARY, false)
  {
    set_pid_file();
  }

  bool ping()
  {
    size_t limit= 5;
    while (_app.check() and --limit)
    {
      if (ping_drizzled(_port))
      {
        return true;
      }
      libtest::dream(1, 0);
    }

    return false;
  }

  const char *name()
  {
    return "drizzled";
  };

  void log_file_option(Application& app, const std::string& arg)
  {
  }

  bool has_log_file_option() const
  {
    return true;
  }

  bool broken_pid_file()
  {
    return true;
  }

  bool is_libtool()
  {
    return false;
  }

  bool has_syslog() const
  {
    return true;
  }

  bool has_port_option() const
  {
    return true;
  }

  void port_option(Application& app, in_port_t arg)
  {
    if (arg > 0)
    {
      char buffer[1024];
      snprintf(buffer, sizeof(buffer), "--drizzle-protocol.port=%d", int(arg));
      app.add_option(buffer);
    }
  }

  bool build(size_t argc, const char *argv[]);
};

bool Drizzle::build(size_t argc, const char *argv[])
{
  if (getuid() == 0 or geteuid() == 0)
  {
    add_option("--user=root");
  }

  add_option("--verbose=INSPECT");
#if 0
  add_option("--datadir=var/drizzle");
#endif

  for (size_t x= 0 ; x < argc ; x++)
  {
    add_option(argv[x]);
  }

  return true;
}

namespace libtest {

libtest::Server *build_drizzled(const char *hostname, in_port_t try_port)
{
  return new Drizzle(hostname, try_port);
}

}
