#include "socket.hpp"

#if defined _WIN32
int get_socket_errno() {
    int local_errno = WSAGetLastError();

  switch (local_errno) {
  case WSAEINVAL:
    local_errno = EINPROGRESS;
    break;
  case WSAEALREADY:
  case WSAEWOULDBLOCK:
    local_errno = EAGAIN;
    break;

  case WSAECONNREFUSED:
    local_errno = ECONNREFUSED;
    break;

  case WSAENETUNREACH:
    local_errno = ENETUNREACH;
    break;

  case WSAETIMEDOUT:
    local_errno = ETIMEDOUT;
    break;

  case WSAECONNRESET:
    local_errno = ECONNRESET;
    break;

  case WSAEADDRINUSE:
    local_errno = EADDRINUSE;
    break;

  case WSAEOPNOTSUPP:
    local_errno = EOPNOTSUPP;
    break;

  case WSAENOPROTOOPT:
    local_errno = ENOPROTOOPT;
    break;

  default:
    break;
  }

  return local_errno;
}
#endif // _WIN32
