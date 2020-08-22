#include "WaitForConn.hpp"

#include <cerrno>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>

bool WaitForConn::connect(const conn_t &conn) {
  int sock;
  unsigned size;
  sockaddr_storage addr;

  tie(sock, addr, size) = conn;

  connect_again:
  if (0 == ::connect(sock, reinterpret_cast<sockaddr *>(&addr), size)) {
    close(sock);
    return true;
  }

  switch (errno) {
    case EINTR:
      goto connect_again;
    case EAGAIN:
    case EALREADY:
    case EINPROGRESS:
    case EISCONN:
    case ETIMEDOUT:
      break;
    default:
      perror("WaitForConn::connect connect()");
      close(sock);
      return false;
  }

  return true;
}

bool WaitForConn::operator () () {
  vector<int> fds;

  fds.reserve(conns.size());
  for (const auto &conn : conns) {
    if (!connect(conn)) {
      return false;
    }
    fds.push_back(get<0>(conn));
  }

  return poll(fds);
}
