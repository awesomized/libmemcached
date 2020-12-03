/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include "common.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

class Connection {
public:
  explicit Connection(socket_or_port_t socket_or_port);
  ~Connection();

  friend void swap(Connection &a, Connection &b);
  void swap(Connection &conn);

  Connection(const Connection &conn);
  Connection &operator=(const Connection &conn);

  Connection(Connection &&conn) noexcept;
  Connection &operator=(Connection &&conn) noexcept;

  int getError();
  int getLastError();

  bool isWritable();
  bool isOpen();

  bool open();
  void close();

private:
  int sock{-1}, last_err{-1};
  sockaddr_storage addr{};
  enum sockaddr_size {
    NONE = 0,
    UNIX = sizeof(sockaddr_un),
    INET = sizeof(sockaddr_in),
    INET6 = sizeof(sockaddr_in6)
  } size = NONE;
  bool connected{false};

  static string error(const initializer_list<string> &args);
};
