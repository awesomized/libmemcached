#pragma once

#include "common.hpp"

#include <sys/socket.h>
#include <sys/un.h>

class Connection {
public:
  explicit Connection(socket_or_port_t socket_or_port);
  ~Connection();

  friend void swap(Connection &a, Connection &b);
  void swap(Connection &conn);

  Connection(const Connection &conn);
  Connection &operator = (const Connection &conn);

  Connection(Connection &&conn) noexcept ;
  Connection &operator = (Connection &&conn) noexcept ;

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
  } size;
  bool connected{false};

  static string error(const initializer_list<string> &args);
};
