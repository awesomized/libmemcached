#include "Connection.hpp"
#include "p9y/poll.hpp"

#include <cerrno>
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#if !(HAVE_SOCK_NONBLOCK && HAVE_SOCK_CLOEXEC)
#  include <fcntl.h>
#  define SOCK_NONBLOCK O_NONBLOCK
#  define SOCK_CLOEXEC O_CLOEXEC
#endif

static inline int socket_ex(int af, int so, int pf, int fl) {
#if HAVE_SOCK_NONBLOCK && HAVE_SOCK_CLOEXEC
  return socket(af, so | fl, pf);
#else
  auto sock = socket(af, so, pf);
  if (0 <= sock) {
    if (0 > fcntl(sock, F_SETFL, fl | fcntl(sock, F_GETFL))) {
      close(sock);
      sock = -1;
    }
  }
  return sock;
#endif
}

Connection::Connection(socket_or_port_t socket_or_port) {
  if (holds_alternative<string>(socket_or_port)) {
    const auto path = get<string>(socket_or_port);
    const auto safe = path.c_str();
    const auto zlen = path.length() + 1;
    const auto ulen = sizeof(sockaddr_un) - sizeof(sa_family_t);

    if (zlen >= ulen) {
      throw invalid_argument(error({"socket(): path too long '", path, "'"}));
    }

    if (0 > (sock = socket_ex(AF_UNIX, SOCK_STREAM, 0, SOCK_NONBLOCK|SOCK_CLOEXEC))) {
      throw runtime_error(error({"socket(): ", strerror(errno)}));
    }

    auto sa = reinterpret_cast<sockaddr_un *>(&addr);
    sa->sun_family = AF_UNIX;
    copy(safe, safe + zlen, sa->sun_path);

    size = UNIX;

  } else {
    if (0 > (sock = socket_ex(AF_INET6, SOCK_STREAM, 0, SOCK_NONBLOCK|SOCK_CLOEXEC))) {
      throw runtime_error(error({"socket(): ", strerror(errno)}));
    }

    const auto port = get<int>(socket_or_port);
    auto sa = reinterpret_cast<struct sockaddr_in6 *>(&addr);
    sa->sin6_family = AF_INET6;
    sa->sin6_port = htons(static_cast<unsigned short>(port));
    sa->sin6_addr = IN6ADDR_LOOPBACK_INIT;

    size = INET6;
  }
}

Connection::~Connection() {
  close();
}

void swap(Connection &a, Connection &b) {
  a.swap(b);
}

void Connection::swap(Connection &conn) {
  Connection copy(conn);
  conn.sock = sock;
  conn.addr = addr;
  conn.size = size;
  conn.last_err = last_err;
  sock = exchange(copy.sock, -1);
  addr = copy.addr;
  size = copy.size;
  last_err = copy.last_err;
}

Connection::Connection(const Connection &conn) {
  if (conn.sock > -1) {
    sock = dup(conn.sock);
  }
  addr = conn.addr;
  size = conn.size;
  last_err = conn.last_err;
}

Connection &Connection::operator=(const Connection &conn) {
  Connection copy(conn);
  copy.swap(*this);
  return *this;
}

Connection::Connection(Connection &&conn) noexcept {
  close();
  swap(conn);
}

Connection &Connection::operator=(Connection &&conn) noexcept {
  Connection copy(move(conn));
  copy.swap(*this);
  return *this;
}

void Connection::close() {
  if (sock > -1) {
    ::close(sock);
    sock = -1;
    last_err = -1;
  }
}

int Connection::getError() {
  int err = -1;
  socklen_t len = sizeof(int);
  if (sock > -1) {
    errno = 0;
    if (0 > getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len)) {
      err = errno;
    }
  }
  last_err = err;
  return err;
}

int Connection::getLastError() {
  if (last_err == -1) {
    return getError();
  }
  return last_err;
}

bool Connection::isWritable() {
  pollfd fd{sock, POLLOUT, 0};
  if (1 > poll(&fd, 1, 0)) {
    return false;
  }
  if (fd.revents & (POLLNVAL|POLLERR|POLLHUP)) {
    return false;
  }
  return fd.revents & POLLOUT;
}

bool Connection::isOpen() {
  if (sock > -1){
    if (isWritable()) {
      return getError() == 0;
    } else if (open()) {
      if (isWritable()) {
        return getError() == 0;
      }
    }
  }
  return false;
}

bool Connection::open() {
  if (connected) {
    return true;
  }
  connect_again:
  errno = 0;
  if (0 == ::connect(sock, reinterpret_cast<sockaddr *>(&addr), size)) {
    connected = true;
    return true;
  }

  switch (errno) {
    case EINTR:
      goto connect_again;
    case EISCONN:
      connected = true;
      [[fallthrough]];
    case EAGAIN:
    case EALREADY:
    case EINPROGRESS:
      return true;

    default:
      return false;
  }
}

string Connection::error(const initializer_list<string> &args) {
  stringstream ss;

  for (const auto &arg : args) {
    ss << arg;
  }

  return ss.str();
}
