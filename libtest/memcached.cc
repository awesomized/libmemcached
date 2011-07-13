/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker All rights reserved.
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


/*
  Startup, and shutdown the memcached servers.
*/

#define TEST_PORT_BASE MEMCACHED_DEFAULT_PORT+10

#include <libtest/common.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <limits.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>

#include <libmemcached/memcached.h>
#include <libmemcached/util.h>

#include <libtest/server.h>
#include <libtest/killpid.h>
#include <libtest/wait.h>

#define SOCKET_FILE "/tmp/memcached.socket"

static pid_t __getpid(server_st& server)
{
  memcached_return_t rc;
  pid_t pid= libmemcached_util_getpid(server.hostname(), server.port(), &rc);
  return pid;
}

static bool __ping(server_st& server)
{
  memcached_return_t rc;
  bool ret= libmemcached_util_ping(server.hostname(), server.port(), &rc);
  return ret;
}

static bool cycle_server(server_st  *server)
{
  while (1)
  {
    if (libmemcached_util_ping(server->hostname(), server->port(), NULL))
    {
      // First we try to kill it, and on fail of that we flush it.
      pid_t pid= libmemcached_util_getpid(server->hostname(), server->port(), NULL);

      if (pid > 0 and kill_pid(pid))
      {
        Error << "Killed existing server," << *server << " with pid:" << pid;
        continue;
      }
      else if (libmemcached_util_flush(server->hostname(), server->port(), NULL)) // If we can flush it, we will just use it
      { 
        Error << "Found server on port " << int(server->port()) << ", flushed it!";
        server->set_used();
        return true;
      } // No idea what is wrong here, so we need to find a different port
      else
      {
        return false;
      }
    }

    break;
  }

  return true;
}

bool server_startup(server_startup_st *construct)
{
  Logn();

  if (getenv(((char *)"MEMCACHED_SERVERS")))
  {
    construct->server_list= getenv(((char *)"MEMCACHED_SERVERS"));
    Log << "MEMCACHED_SERVERS " << construct->server_list;
    construct->count= 0;
  }
  else
  {
    std::string server_config_string;

    uint32_t port_base= 0;
    for (uint32_t x= 0; x < uint32_t(construct->count -1); x++)
    {
      server_st *server= NULL;

      {
        char *var;
        char variable_buffer[1024];

        snprintf(variable_buffer, sizeof(variable_buffer), "LIBMEMCACHED_PORT_%u", x);

        if ((var= getenv(variable_buffer)))
        {
          server= new server_st((in_port_t)atoi(var), __getpid, __ping);
        }
        else
        {
          server= new server_st(in_port_t(x +TEST_PORT_BASE +port_base), __getpid, __ping);

          while (not cycle_server(server))
          {
            Error << "Found server " << *server << ", could not flush it, so trying next port.";
            port_base++;
            server->set_port(in_port_t(x +TEST_PORT_BASE +port_base));
          }
        }
      }

      if (server->is_used())
      {
        Log << "Using server at : " << server;
      }
      else
      {
        char buffer[FILENAME_MAX];
        if (x == 0)
        {
          snprintf(buffer, sizeof(buffer), "%s -d -t 1 -p %u -U %u -m 128",
                   MEMCACHED_BINARY, server->port(), server->port());
        }
        else
        {
          snprintf(buffer, sizeof(buffer), "%s -d -t 1 -p %u -U %u",
                   MEMCACHED_BINARY, server->port(), server->port());
        }
        server->set_command(buffer);

        if (not server->start())
        {
          Error << "Failed system(" << buffer << ")";
          delete server;
          return false;
        }
        Log << "STARTING SERVER: " << buffer << " pid:" << server->pid();
      }
      construct->push_server(server);

      if (x == 0)
      {
        assert(server->has_port());
        set_default_port(server->port());
      }

      char port_str[NI_MAXSERV];
      snprintf(port_str, sizeof(port_str), "%u", int(server->port()));

      server_config_string+= "--server=";
      server_config_string+= server->hostname();
      server_config_string+= ":";
      server_config_string+= port_str;
      server_config_string+= " ";
    }

    // Socket
    {

      std::string socket_file(SOCKET_FILE);
      char *var;

      if ((var= getenv("LIBMEMCACHED_SOCKET")))
      {
        socket_file= var;
      }

      server_st *server= new server_st(SOCKET_FILE, __getpid, __ping);

      if (not cycle_server(server))
      {
        Error << "Found server " << server << ", could not flush it, failing since socket file is not available.";
        return false;
      }

      if (server->is_used())
      {
        Log << "Using server at : " << *server;
      }
      else
      {
        char buffer[FILENAME_MAX];
        snprintf(buffer, sizeof(buffer), "%s -d -t 1 -s %s", MEMCACHED_BINARY, SOCKET_FILE);
        server->set_command(buffer);

        if (not server->start())
        {
          Error << "Failed system(" << buffer << ")";
          delete server;
          return false;
        }
        Log << "STARTING SERVER: " << buffer << " pid:" << server->pid();
      }
      set_default_socket(server->hostname());
      construct->push_server(server);

      {
        server_config_string+= "--socket=\"";
        server_config_string+= server->hostname();
        server_config_string+= "\" ";
      }
    }

    server_config_string.resize(server_config_string.size() -1); // Remove final space
    construct->server_list= server_config_string;
  }

  Logn();

  srandom((unsigned int)time(NULL));

  return true;
}
