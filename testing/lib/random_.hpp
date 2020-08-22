#pragma once

#include <string>
#include <tuple>
#include "Server.hpp"

using namespace std;

unsigned random_num(unsigned min, unsigned max);
unsigned random_port();
string random_socket();
string random_socket_or_port_string(const string &what);
string random_socket_or_port_flag();

inline Server::dyn_args_t::value_type random_socket_or_port_arg() {
  return {random_socket_or_port_flag(), &random_socket_or_port_string};
}
