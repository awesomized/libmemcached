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

#pragma once

#include <cassert>
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace libtest {

class server_startup_st
{
private:
  uint64_t _magic;
  std::string server_list;
  bool _socket;
  bool _sasl;
  uint32_t _count;
  std::string _username;
  std::string _password;

public:

  uint8_t udp;
  std::vector<Server *> servers;

  server_startup_st();
  ~server_startup_st();

  bool validate();

  bool start_socket_server(const std::string& server_type, const in_port_t try_port, int argc, const char *argv[]);

  uint32_t count() const
  {
    return servers.size();
  }

  void restart();

  std::string option_string() const;

  const std::string& password() const
  {
    return _password;
  }

  const std::string& username() const
  {
    return _username;
  }

  bool socket()
  {
    return _socket;
  }

  bool sasl()
  {
    return _sasl;
  }

  void set_socket()
  {
    _socket= true;
  }

  void set_sasl(const std::string& username_arg, const std::string& password_arg)
  {
    _sasl= true;
    _username= username_arg;
    _password= password_arg;
  }


  void shutdown_and_remove();
  bool shutdown();
  bool shutdown(uint32_t number_of_host);

  bool check() const;

  void push_server(Server *);
  Server *pop_server();

  unsigned long int servers_to_run() const
  {
    return _servers_to_run;
  }

  void set_servers_to_run(unsigned long int arg)
  {
    _servers_to_run= arg;
  }

private:
  unsigned long int _servers_to_run;
};

bool server_startup(server_startup_st&, const std::string&, in_port_t try_port, int argc, const char *argv[], const bool opt_startup_message= true);

} // namespace libtest
