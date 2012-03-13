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

#include <libmemcached-1.0/memcached.h>
#include <libmemcachedutil-1.0/util.h>

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

#include <libtest/server.h>
#include <libtest/wait.h>

#include <libtest/memcached.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

using namespace libtest;

class Memcached : public libtest::Server
{
  std::string _username;
  std::string _password;

public:
  Memcached(const std::string& host_arg, const in_port_t port_arg, const bool is_socket_arg, const std::string& username_arg, const std::string& password_arg) :
    libtest::Server(host_arg, port_arg, is_socket_arg),
    _username(username_arg),
    _password(password_arg)
  { }

  Memcached(const std::string& host_arg, const in_port_t port_arg, const bool is_socket_arg) :
    libtest::Server(host_arg, port_arg, is_socket_arg)
  {
    set_pid_file();
  }

  virtual const char *sasl() const
  {
    return NULL;
  }

  const std::string& password() const
  {
    return _password;
  }

  const std::string& username() const
  {
    return _username;
  }

  pid_t get_pid(bool error_is_ok)
  {
    // Memcached is slow to start, so we need to do this
    if (pid_file().empty() == false)
    {
      if (error_is_ok and
          wait_for_pidfile() == false)
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;
      }
    }

    pid_t local_pid;
    memcached_return_t rc= MEMCACHED_SUCCESS;
    if (has_socket())
    {
      if (socket().empty())
      {
        return -1;
      }

      local_pid= libmemcached_util_getpid(socket().c_str(), port(), &rc);
    }
    else
    {
      local_pid= libmemcached_util_getpid(hostname().c_str(), port(), &rc);
    }

    if (error_is_ok and ((memcached_failed(rc) or not is_pid_valid(local_pid))))
    {
      Error << "libmemcached_util_getpid(" << memcached_strerror(NULL, rc) << ") pid: " << local_pid << " for:" << *this;
    }

    return local_pid;
  }

  bool ping()
  {
    // Memcached is slow to start, so we need to do this
    if (pid_file().empty() == false)
    {
      if (wait_for_pidfile() == false)
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;
      }
    }

    memcached_return_t rc;
    bool ret;

    if (has_socket())
    {
      ret= libmemcached_util_ping(socket().c_str(), 0, &rc);
    }
    else
    {
      ret= libmemcached_util_ping(hostname().c_str(), port(), &rc);
    }

    if (memcached_failed(rc) or not ret)
    {
      Error << "libmemcached_util_ping(" << hostname() << ", " << port() << ") error: " << memcached_strerror(NULL, rc);
    }

    return ret;
  }

  const char *name()
  {
    return "memcached";
  };

  const char *executable()
  {
    return MEMCACHED_BINARY;
  }

  virtual void pid_file_option(Application& app, const std::string& arg)
  {
    if (arg.empty() == false)
    {
      app.add_option("-P", arg);
    }
  }

  const char *socket_file_option() const
  {
    return "-s ";
  }

  const char *daemon_file_option()
  {
    return "-d";
  }

  virtual void port_option(Application& app, in_port_t arg)
  {
    char buffer[30];
    snprintf(buffer, sizeof(buffer), "%d", int(arg));
    app.add_option("-p", buffer); 
  }

  bool has_port_option() const
  {
    return true;
  }

  bool has_socket_file_option() const
  {
    return has_socket();
  }

  void socket_file_option(Application& app, const std::string& socket_arg)
  {
    if (socket_arg.empty() == false)
    {
      app.add_option("-s", socket_arg);
    }
  }

  bool is_libtool()
  {
    return false;
  }

  bool broken_socket_cleanup()
  {
    return true;
  }

  // Memcached's pidfile is broken
  bool broken_pid_file()
  {
    return true;
  }

  bool build(size_t argc, const char *argv[]);
};

class MemcachedLight : public libtest::Server
{

public:
  MemcachedLight(const std::string& host_arg, const in_port_t port_arg):
    libtest::Server(host_arg, port_arg)
  {
    set_pid_file();
  }

  pid_t get_pid(bool error_is_ok)
  {
    // Memcached is slow to start, so we need to do this
    if (pid_file().empty() == false)
    {
      if (error_is_ok and wait_for_pidfile() == false)
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;
      }
    }

    bool success= false;
    std::stringstream error_message;
    pid_t local_pid= get_pid_from_file(pid_file(), error_message);
    if (local_pid > 0)
    {
      if (::kill(local_pid, 0) > 0)
      {
        success= true;
      }
    }

    if (error_is_ok and ((success or not is_pid_valid(local_pid))))
    {
      Error << "kill(" << " pid: " << local_pid << " errno:" << strerror(errno) << " for:" << *this;
    }

    return local_pid;
  }

  bool ping()
  {
    // Memcached is slow to start, so we need to do this
    if (not pid_file().empty())
    {
      if (not wait_for_pidfile())
      {
        Error << "Pidfile was not found:" << pid_file();
        return false;
      }
    }

    std::stringstream error_message;
    pid_t local_pid= get_pid_from_file(pid_file(), error_message);
    if (local_pid > 0)
    {
      if (::kill(local_pid, 0) == 0)
      {
        return true;
      }
    }

    return false;
  }

  const char *name()
  {
    return "memcached_light";
  };

  const char *executable()
  {
    return MEMCACHED_LIGHT_BINARY;
  }

  const char *daemon_file_option()
  {
    return "--daemon";
  }

  virtual void port_option(Application& app, in_port_t arg)
  {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "--port=%d", int(arg));
    app.add_option(buffer);
  }

  bool has_port_option() const
  {
    return true;
  }

  bool is_libtool()
  {
    return true;
  }

  void log_file_option(Application& app, const std::string& arg)
  {
    if (arg.empty() == false)
    {
      std::string buffer("--log-file=");
      buffer+= arg;
      app.add_option("--verbose");
      app.add_option(buffer);
    }
  }

  bool has_log_file_option() const
  {
    return true;
  }

  bool build(size_t argc, const char *argv[]);
};

class MemcachedSaSL : public Memcached
{
public:
  MemcachedSaSL(const std::string& host_arg,
                const in_port_t port_arg, 
                const bool is_socket_arg, 
                const std::string& username_arg, 
                const std::string &password_arg) :
    Memcached(host_arg, port_arg, is_socket_arg, username_arg, password_arg)
  { }

  const char *name()
  {
    return "memcached-sasl";
  };

  const char *sasl() const
  {
    return " -S -B binary ";
  }

  const char *executable()
  {
    return MEMCACHED_SASL_BINARY;
  }

  pid_t get_pid(bool error_is_ok)
  {
    // Memcached is slow to start, so we need to do this
    if (pid_file().empty() == false)
    {
      if (error_is_ok and 
          wait_for_pidfile() == false)
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;
      }
    }

    pid_t local_pid;
    memcached_return_t rc;
    if (has_socket())
    {
      local_pid= libmemcached_util_getpid2(socket().c_str(), 0, username().c_str(), password().c_str(), &rc);
    }
    else
    {
      local_pid= libmemcached_util_getpid2(hostname().c_str(), port(), username().c_str(), password().c_str(), &rc);
    }

    if (error_is_ok and ((memcached_failed(rc) or not is_pid_valid(local_pid))))
    {
      Error << "libmemcached_util_getpid2(" << memcached_strerror(NULL, rc) << ") username: " << username() << " password: " << password() << " pid: " << local_pid << " for:" << *this;
    }

    return local_pid;
  }

  bool ping()
  {
    // Memcached is slow to start, so we need to do this
    if (not pid_file().empty())
    {
      if (not wait_for_pidfile())
      {
        Error << "Pidfile was not found:" << pid_file();
        return -1;
      }
    }

    memcached_return_t rc;
    bool ret;

    if (has_socket())
    {
      ret= libmemcached_util_ping2(socket().c_str(), 0, username().c_str(), password().c_str(), &rc);
    }
    else
    {
      ret= libmemcached_util_ping2(hostname().c_str(), port(), username().c_str(), password().c_str(), &rc);
    }

    if (memcached_failed(rc) or not ret)
    {
      Error << "libmemcached_util_ping2(" << hostname() << ", " << port() << ", " << username() << ", " << password() << ") error: " << memcached_strerror(NULL, rc);
    }

    return ret;
  }

};


#include <sstream>

bool Memcached::build(size_t argc, const char *argv[])
{
  std::stringstream arg_buffer;

  if (getuid() == 0 or geteuid() == 0)
  {
    add_option("-u", "root");
  }

  add_option("-l", "localhost");
  add_option("-m", "128");
  add_option("-M");

  if (sasl())
  {
    add_option(sasl());
  }

  for (int x= 0 ; x < argc ; x++)
  {
    add_option(argv[x]);
  }

  return true;
}

bool MemcachedLight::build(size_t argc, const char *argv[])
{
  for (size_t x= 0 ; x < argc ; x++)
  {
    add_option(argv[x]);
  }

  return true;
}

namespace libtest {

libtest::Server *build_memcached(const std::string& hostname, const in_port_t try_port)
{
  return new Memcached(hostname, try_port, false);
}

libtest::Server *build_memcached_socket(const std::string& socket_file, const in_port_t try_port)
{
  return new Memcached(socket_file, try_port, true);
}

libtest::Server *build_memcached_light(const std::string& hostname, const in_port_t try_port)
{
  return new MemcachedLight(hostname, try_port);
}


libtest::Server *build_memcached_sasl(const std::string& hostname, const in_port_t try_port, const std::string& username, const std::string &password)
{
  if (username.empty())
  {
    return new MemcachedSaSL(hostname, try_port, false,  "memcached", "memcached");
  }

  return new MemcachedSaSL(hostname, try_port, false,  username, password);
}

libtest::Server *build_memcached_sasl_socket(const std::string& socket_file, const in_port_t try_port, const std::string& username, const std::string &password)
{
  if (username.empty())
  {
    return new MemcachedSaSL(socket_file, try_port, true, "memcached", "memcached");
  }

  return new MemcachedSaSL(socket_file, try_port, true, username, password);
}

}

