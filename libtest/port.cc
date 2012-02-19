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

static in_port_t global_port= 0;
static in_port_t global_max_port= 0;

namespace libtest {

in_port_t default_port()
{
  return global_port;
}
 
void set_default_port(in_port_t port)
{
  global_port= port;
}

in_port_t max_port()
{
  return global_max_port;
}
 
void set_max_port(in_port_t port)
{
  if (port > global_max_port)
  {
    global_max_port= port;
  }

  global_max_port= port;
}

in_port_t get_free_port()
{
  in_port_t ret_port= in_port_t(0);
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

        if (listen(sd, 100) != -1)
        {
          if (getsockname(sd, (struct sockaddr *)&sin, &addrlen) != -1)
          {
            ret_port= sin.sin_port;
          }
        }
      }
    }

    close(sd);
  }

  return ret_port;
}

} // namespace libtest
