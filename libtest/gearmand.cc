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

#include <libtest/gearmand.h>

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

#include <libgearman/gearman.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

class GetPid : public util::Instance::Finish
{
private:
  pid_t _pid;

public:
  GetPid() :
    _pid(-1)
  { }

  pid_t pid()
  {
    return _pid;
  }


  bool call(const bool success, const std::string &response)
  {
    _pid= -1;
    if (success and response.size())
    {
      _pid= atoi(response.c_str());
    }

    if (is_pid_valid(_pid) == false)
    {
      _pid= -1;
      return false;
    }

    return true;
  }
};

using namespace libtest;

class Gearmand : public libtest::Server
{
private:
public:
  Gearmand(const std::string& host_arg, in_port_t port_arg) :
    libtest::Server(host_arg, port_arg, GEARMAND_BINARY, true)
  {
    set_pid_file();
  }

  pid_t get_pid(bool error_is_ok)
  {
    if (pid_file().empty() == false)
    {
      Wait wait(pid_file(), 0);

      if (error_is_ok and not wait.successful())
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;
      }
    }

    GetPid *get_instance_pid;
    util::Instance instance(hostname(), port());
    instance.set_finish(get_instance_pid= new GetPid);

    instance.push(new util::Operation(test_literal_param("getpid\r\n"), true));

    if (error_is_ok and instance.run() == false)
    {
      Error << "Failed to obtain pid of server";
    }

    return get_instance_pid->pid();
  }

  bool ping()
  {
    gearman_client_st *client= gearman_client_create(NULL);
    if (client == NULL)
    {
      Error << "Could not allocate memory for gearman_client_create()";
      return false;
    }
    gearman_client_set_timeout(client, 3000);

    if (gearman_success(gearman_client_add_server(client, hostname().c_str(), port())))
    {
      gearman_return_t rc= gearman_client_echo(client, test_literal_param("This is my echo test"));

      if (gearman_success(rc))
      {
        gearman_client_free(client);
        return true;
      }
#if 0
      Error << hostname().c_str() << ":" << port() << " was " << gearman_strerror(rc) << " extended: " << gearman_client_error(client);
#endif
    }
    else
    {
      Error << "gearman_client_add_server() " << gearman_client_error(client);
    }

    gearman_client_free(client);

    return false;;
  }

  const char *name()
  {
    return "gearmand";
  };

  void log_file_option(Application& app, const std::string& arg)
  {
    if (arg.empty() == false)
    {
      std::string buffer("--log-file=");
      buffer+= arg;
      app.add_option("--verbose=DEBUG");
      app.add_option(buffer);
    }
  }

  bool has_log_file_option() const
  {
    return true;
  }

  bool is_libtool()
  {
    return true;
  }

  bool has_syslog() const
  {
    return true;
  }

  bool has_port_option() const
  {
    return true;
  }

  bool build(size_t argc, const char *argv[]);
};

bool Gearmand::build(size_t argc, const char *argv[])
{
  std::stringstream arg_buffer;

  if (getuid() == 0 or geteuid() == 0)
  {
    add_option("-u", "root");
  }

  add_option("--listen=localhost");

  for (size_t x= 0 ; x < argc ; x++)
  {
    add_option(argv[x]);
  }

  return true;
}

namespace libtest {

libtest::Server *build_gearmand(const char *hostname, in_port_t try_port)
{
  return new Gearmand(hostname, try_port);
}

}
