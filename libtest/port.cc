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

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fnmatch.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <unistd.h>

#include <utility>
#include <vector>

#include <signal.h>

#include <libtest/signal.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

using namespace libtest;

struct socket_st {
  typedef std::vector< std::pair< int, in_port_t> > socket_port_t;
  socket_port_t _pair;

  void release(in_port_t _arg)
  {
    for(socket_port_t::iterator iter= _pair.begin();
        iter != _pair.end();
        iter++)
    {
      if ((*iter).second == _arg)
      {
        shutdown((*iter).first, SHUT_RDWR);
        close((*iter).first);
      }
    }
  }

  ~socket_st()
  {
    for(socket_port_t::iterator iter= _pair.begin();
        iter != _pair.end();
        iter++)
    {
      shutdown((*iter).first, SHUT_RDWR);
      close((*iter).first);
    }
  }
};

static socket_st all_socket_fd;

static in_port_t global_port= 0;

namespace libtest {

in_port_t default_port()
{
  if (global_port == 0)
  {
    global_port= get_free_port();
  }

  return global_port;
}

void release_port(in_port_t arg)
{
  all_socket_fd.release(arg);
}

in_port_t get_free_port()
{
  in_port_t ret_port= in_port_t(0);

  int retries= 1024;

  while (retries--)
  {
    int sd;
    if ((sd= socket(AF_INET, SOCK_STREAM, 0)) != -1)
    {
      int optval= 1;
      if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != -1)
      {
        struct sockaddr_in sin;
        sin.sin_port= 0;
        sin.sin_addr.s_addr= 0;
        sin.sin_addr.s_addr= INADDR_ANY;
        sin.sin_family= AF_INET;

        if (bind(sd, (struct sockaddr *)&sin,sizeof(struct sockaddr_in) ) != -1)
        {
          socklen_t addrlen= sizeof(sin);

          if (getsockname(sd, (struct sockaddr *)&sin, &addrlen) != -1)
          {
            ret_port= sin.sin_port;
          }
        }
      }

      all_socket_fd._pair.push_back(std::make_pair(sd, ret_port));
    }

    if (ret_port > 1024)
    {
      break;
    }
  }

  // We handle the case where if we max out retries, we still abort.
  if (ret_port <= 1024)
  {
    fatal_message("No port could be found");
  }

  return ret_port;
}

} // namespace libtest
