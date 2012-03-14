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

#include <signal.h>

#include <libtest/stats.h>
#include <libtest/signal.h>

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

using namespace libtest;

struct socket_st {
  std::vector<int> fd;

  ~socket_st()
  {
    for(std::vector<int>::iterator iter= fd.begin(); iter != fd.end(); iter++)
    {
      close(*iter);
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

      all_socket_fd.fd.push_back(sd);
    }

    if (ret_port > 1024)
    {
      break;
    }
  }

  // We handle the case where if we max out retries, we still abort.
  if (ret_port <= 1024)
  {
    throw fatal_message("No port could be found");
  }

  return ret_port;
}

} // namespace libtest
