#include "libgen.hpp"
#if defined P9Y_NEED_BASENAME
# if defined _WIN32
# include <cstring>
char *basename(const char *filename) {
  static char base[_MAX_PATH * 2], ext[_MAX_PATH], *ptr;
  (void) _splitpath_s(filename, NULL, 0, NULL, 0, base, _MAX_PATH, ext, _MAX_PATH);
  strcat_s(base, _MAX_PATH * 2 - 1, ext);
  return base;
}
# endif // _WIN32
#endif // P9Y_NEED_BASENAME

#include "realpath.hpp"
#if defined P9Y_NEED_REALPATH
# if defined _WIN32
char *realpath(const char *path, char real[_MAX_PATH]) {
  return _fullpath(real, path, _MAX_PATH);
}
# endif // _WIN32
#endif // P9Y_NEED_REALPATH

#include "gettimeofday.hpp"
#if defined P9Y_NEED_GETTIMEOFDAY
# include <chrono>
int gettimeofday(struct timeval* tp, struct timezone*) {
  using clock = std::chrono::system_clock;
  auto as_sec = [] (auto d) {
      return std::chrono::duration_cast<std::chrono::seconds>(d);
  };
  auto as_usec = [] (auto d) {
      return std::chrono::duration_cast<std::chrono::microseconds>(d);
  };

  auto now = clock::now().time_since_epoch();
  auto sec = as_sec(now);
  auto usec = as_usec(now - sec);

  tp->tv_sec = sec.count();
  tp->tv_usec = usec.count();
  return 0;
}
#endif // P9Y_NEED_GETTIMEOFDAY

#include "socket.hpp"
#if defined P9Y_NEED_GET_SOCKET_ERRNO
# if defined _WIN32
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
# endif // _WIN32
#endif // P9Y_NEED_GET_SOCKET_ERRNO

#include "poll.hpp"
#if defined P9Y_NEED_POLL
int poll(struct pollfd fds[], nfds_t nfds, int tmo) {
  fd_set readfds, writefds, errorfds;
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&errorfds);

  int maxfd = 0;

  for (nfds_t x = 0; x < nfds; ++x) {
    if (fds[x].events & (POLLIN | POLLOUT)) {
#  ifndef _WIN32
      if (fds[x].fd > maxfd) {
        maxfd = fds[x].fd;
      }
#  endif
      if (fds[x].events & POLLIN) {
        FD_SET(fds[x].fd, &readfds);
      }
      if (fds[x].events & POLLOUT) {
        FD_SET(fds[x].fd, &writefds);
      }
    }
  }

  struct timeval timeout = {tmo / 1000, (tmo % 1000) * 1000};
  struct timeval *tp = &timeout;
  if (tmo == -1) {
    tp = NULL;
  }
  int ret = select(maxfd + 1, &readfds, &writefds, &errorfds, tp);
  if (ret <= 0) {
    return ret;
  }

  /* Iterate through all of them because I need to clear the revent map */
  for (nfds_t x = 0; x < nfds; ++x) {
    fds[x].revents = 0;
    if (FD_ISSET(fds[x].fd, &readfds)) {
      fds[x].revents |= POLLIN;
    }
    if (FD_ISSET(fds[x].fd, &writefds)) {
      fds[x].revents |= POLLOUT;
    }
    if (FD_ISSET(fds[x].fd, &errorfds)) {
      fds[x].revents |= POLLERR;
    }
  }

  return ret;
}
#endif // P9Y_NEED_POLL

#include "clock_gettime.hpp"
#ifdef P9Y_NEED_CLOCK_GETTIME
# ifdef _WIN32

static inline __int64 wintime2unixtime(__int64 wintime) {
  return wintime - 116444736000000000i64;
}

int clock_gettime(int, struct timespec *spec)
{
  __int64 wintime, unixtime;

  GetSystemTimeAsFileTime((FILETIME*) &wintime);
  unixtime = wintime2unixtime(wintime);


  spec->tv_sec = unixtime / 10000000i64;
  spec->tv_nsec = unixtime % 10000000i64 * 100;

  return 0;
}
# endif // _WIN32
#endif // P9Y_NEED_CLOCK_GETTIME
