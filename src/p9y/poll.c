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
