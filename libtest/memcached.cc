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

#include <cassert>
#include <cerrno>
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

#include <boost/lexical_cast.hpp>


#define CERR_PREFIX std::endl << __FILE__ << ":" << __LINE__ << " "

static void global_sleep(void)
{
  static struct timespec global_sleep_value= { 0, 50000 };

#ifdef WIN32
  sleep(1);
#else
  nanosleep(&global_sleep_value, NULL);
#endif
}

#define SOCKET_FILE "/tmp/memcached.socket"

static bool cycle_server(server_st& server)
{
  while (1)
  {
    if (libmemcached_util_ping(server.hostname, server.port(), NULL))
    {
      // First we try to kill it, and on fail of that we flush it.
      pid_t pid= libmemcached_util_getpid(server.hostname, server.port(), NULL);

      if (pid > 0 and kill_pid(pid))
      {
        std::cerr << CERR_PREFIX << "Killed existing server," << server << " with pid:" << pid << std::endl;
        continue;
      }
      else if (libmemcached_util_flush(server.hostname, server.port(), NULL)) // If we can flush it, we will just use it
      { 
        std::cerr << CERR_PREFIX << "Found server on port " << int(server.port()) << ", flushed it!" << std::endl;
        server.set_used();
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
  if (getenv(((char *)"MEMCACHED_SERVERS")))
  {
    construct->server_list= getenv(((char *)"MEMCACHED_SERVERS"));
    printf("servers %s\n", construct->server_list.c_str());
    construct->count= 0;
  }
  else
  {
    std::string server_config_string;

    uint32_t port_base= 0;
    for (uint32_t x= 0; x < (construct->count -1); x++)
    {
      server_st &server= construct->server[x];

      {
        char *var;
        char variable_buffer[1024];

        snprintf(variable_buffer, sizeof(variable_buffer), "LIBMEMCACHED_PORT_%u", x);

        if ((var= getenv(variable_buffer)))
        {
          server.set_port((in_port_t)atoi(var));
        }
        else
        {
          server.set_port(in_port_t(x + TEST_PORT_BASE + port_base));

          while (not cycle_server(server))
          {
            std::cerr << CERR_PREFIX << "Found server " << server << ", could not flush it, so trying next port." << std::endl;
            port_base++;
            server.set_port(in_port_t(x + TEST_PORT_BASE + port_base));
          }
        }
      }

      if (server.is_used())
      {
        std::cerr << std::endl << "Using server at : " << server << std::endl;
      }
      else
      {
        char buffer[FILENAME_MAX];
        if (x == 0)
        {
          snprintf(buffer, sizeof(buffer), "%s -d -t 1 -p %u -U %u -m 128",
                   MEMCACHED_BINARY, server.port(), server.port());
        }
        else
        {
          snprintf(buffer, sizeof(buffer), "%s -d -t 1 -p %u -U %u",
                   MEMCACHED_BINARY, server.port(), server.port());
        }

        int status= system(buffer);
        if (status == -1)
        {
          std::cerr << CERR_PREFIX << "Failed system(" << buffer << ")" << std::endl;
          return false;
        }
        fprintf(stderr, "STARTING SERVER: %s\n", buffer);

        int count= 30;
        memcached_return_t rc;
        while (not libmemcached_util_ping(server.hostname, server.port(), &rc) and --count)
        {
          global_sleep();
        }

        if (memcached_failed(rc))
        {
          std::cerr << CERR_PREFIX << "libmemcached_util_ping() failed:" << memcached_strerror(NULL, rc) << " Connection:" << server << std::endl;
          return false;
        }

        server.set_pid(libmemcached_util_getpid(server.hostname, server.port(), &rc));
        if (not server.has_pid())
        {
          std::cerr << CERR_PREFIX << "libmemcached_util_getpid() failed" << memcached_strerror(NULL, rc) << " Connection: " << server << std::endl;
          return false;
        }
      }

      server_config_string+= "--server=";
      server_config_string+= server.hostname;
      server_config_string+= ":";
      server_config_string+= boost::lexical_cast<std::string>(server.port());
      server_config_string+= " ";
      fprintf(stderr, " Port %d\n", server.port());
    }

    {
      server_st &server= construct->server[construct->count -1];

      {
        std::string socket_file;
        char *var;

        server.set_hostname(SOCKET_FILE);

        if ((var= getenv("LIBMEMCACHED_SOCKET")))
        {
          socket_file+= var;
        }
        else
        {
          if (not cycle_server(server))
          {
            std::cerr << CERR_PREFIX << "Found server " << server << ", could not flush it, so trying next port." << std::endl;
            return false;
          }
        }
      }

      if (server.is_used())
      {
        std::cerr << std::endl << "Using server at : " << server << std::endl;
      }
      else
      {
        char buffer[FILENAME_MAX];
        snprintf(buffer, sizeof(buffer), "%s -d -t 1 -s %s", MEMCACHED_BINARY, SOCKET_FILE);

        int status= system(buffer);
        if (status == -1)
        {
          std::cerr << CERR_PREFIX << "Failed system(" << buffer << ")" << std::endl;
          return false;
        }
        fprintf(stderr, "STARTING SERVER: %s\n", buffer);

        int count= 30;
        memcached_return_t rc;
        while (not libmemcached_util_ping(server.hostname, server.port(), &rc) and --count)
        {
          global_sleep();
        }

        if (memcached_failed(rc))
        {
          std::cerr << CERR_PREFIX << "libmemcached_util_ping() failed:" << memcached_strerror(NULL, rc) << " Connection:" << server << std::endl;
          return false;
        }

        server.set_pid(libmemcached_util_getpid(server.hostname, server.port(), &rc));
        if (not server.has_pid())
        {
          std::cerr << CERR_PREFIX << "libmemcached_util_getpid() failed" << memcached_strerror(NULL, rc) << " Connection: " << server << std::endl;
          return false;
        }
      }

      {
        set_default_socket(server.hostname);
        server_config_string+= "--socket=\"";
        server_config_string+= server.hostname;
        server_config_string+= "\" ";
      }
    }

    server_config_string.resize(server_config_string.size() -1); // Remove final space
    construct->server_list= server_config_string;
  }

  srandom((unsigned int)time(NULL));

  std::cerr << std::endl;
  return true;
}

void server_shutdown(server_startup_st *construct)
{
  for (uint32_t x= 0; x < construct->count; x++)
  {
    if (construct->server[x].is_used())
      continue;

    construct->server[x].kill();
  }
}
