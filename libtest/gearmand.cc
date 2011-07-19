/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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

    if (_pid < 1)
    {
      _pid= -1;
      return false;
    }

    return true;
  }
};

using namespace libtest;

class Gearmand : public Server
{
private:
public:
  Gearmand(const std::string& host_arg, in_port_t port_arg) :
    Server(host_arg, port_arg)
  { }

  pid_t get_pid(bool error_is_ok)
  {
    if (not pid_file().empty())
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
    if (not client)
    {
      Error << "Could not allocate memory for gearman_client_create()";
      return false;
    }
    gearman_client_set_timeout(client, 1000);

    if (gearman_success(gearman_client_add_server(client, hostname().c_str(), port())))
    {
      gearman_return_t rc= gearman_client_echo(client, test_literal_param("This is my echo test"));

      if (gearman_success(rc))
      {
        gearman_client_free(client);
        return true;
      }
    }

    gearman_client_free(client);

    return false;;
  }

  const char *name()
  {
    return "gearmand";
  };

  const char *executable()
  {
    return GEARMAND_BINARY;
  }

  const char *pid_file_option()
  {
    return "--pid-file=";
  }

  const char *daemon_file_option()
  {
    return "--daemon";
  }

  const char *log_file_option()
  {
    return "-vvvvv --log-file=";
  }

  const char *port_option()
  {
    return "--port=";
  }

  bool is_libtool()
  {
    return true;
  }

  bool build(int argc, const char *argv[]);
};


#include <sstream>

bool Gearmand::build(int argc, const char *argv[])
{
  std::stringstream arg_buffer;

  if (getuid() == 0 or geteuid() == 0)
  {
    arg_buffer << " -u root ";
  }

  for (int x= 1 ; x < argc ; x++)
  {
    arg_buffer << " " << argv[x] << " ";
  }

  set_extra_args(arg_buffer.str());

  return true;
}

namespace libtest {

Server *build_gearmand(const char *hostname, in_port_t try_port)
{
  return new Gearmand(hostname, try_port);
}

}
