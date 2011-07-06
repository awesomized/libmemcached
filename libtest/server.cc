/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
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

#include <config.h>
#include <iostream>
#include <cstdlib>
#include <cassert>

#include <libtest/server.h>
#include <libtest/killpid.h>


std::ostream& operator<<(std::ostream& output, const server_st &arg)
{
  if (arg.is_socket())
  {
    output << arg.hostname();
  }
  else
  {
    output << arg.hostname() << ":" << arg.port();
  }
  return output;  // for multiple << operators
}

static void global_sleep(void)
{
  static struct timespec global_sleep_value= { 0, 50000 };

#ifdef WIN32
  sleep(1);
#else
  nanosleep(&global_sleep_value, NULL);
#endif
}

server_st::server_st(in_port_t port_arg, test_server_getpid *get_pid_arg, test_server_ping *ping_arg) :
  _used(false),
  _pid(-1),
  _port(port_arg),
  __get_pid(get_pid_arg),
  __ping(ping_arg),
  _hostname("localhost")
{
  pid_file[0]= 0;
}

server_st::server_st(const std::string &socket_file, test_server_getpid *get_pid_arg, test_server_ping *ping_arg) :
  _used(false),
  _pid(-1),
  _port(0),
  __get_pid(get_pid_arg),
  __ping(ping_arg),
  _hostname(socket_file)
{
  pid_file[0]= 0;
}


server_st::~server_st()
{
  if (has_pid())
  {
    kill();
  }
}

bool server_st::start()
{
  assert(not _command.empty());
  assert(not has_pid());

  if (has_pid())
    return false;

  if (system(_command.c_str()) == -1)
    return false;

  int count= 30;
  while (not ping() and --count)
  {
    global_sleep();
  }

  if (count == 0)
  {
    return false;
  }

  _pid= get_pid();

  return has_pid();
}

void server_st::reset_pid()
{
  pid_file[0]= 0;
  _pid= -1;
}

pid_t server_st::pid()
{
  if (not has_pid())
  {
    _pid= get_pid();
  }

  return _pid;
}


bool server_st::kill()
{
  if (is_used())
    return false;

  if ((_pid= get_pid()))
  {
    kill_pid(_pid);
    if (pid_file[0])
    {
      unlink(pid_file); // If this happens we may be dealing with a dead server that left its pid file.
    }
    reset_pid();

    return true;
  }
#if 0
  else if (pid_file[0])
  {
    kill_file(pid_file);
    reset_pid();

    return true;
  }
#endif

  return false;
}

void server_startup_st::push_server(server_st *arg)
{
  servers.push_back(arg);
}

void server_startup_st::shutdown()
{
  for (std::vector<server_st *>::iterator iter= servers.begin(); iter != servers.end(); iter++)
  {
    if ((*iter)->is_used())
      continue;

    (*iter)->kill();
  }
}

server_startup_st::~server_startup_st()
{
  for (std::vector<server_st *>::iterator iter= servers.begin(); iter != servers.end(); iter++)
  {
    delete *iter;
  }
  servers.clear();
}
