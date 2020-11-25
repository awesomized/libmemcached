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

#include "libmemcached/common.h"

#if defined(_WIN32)
#  include "libmemcached/poll.h"
#  if HAVE_SYS_TIME_H
#    include <sys/time.h>
#  endif
#  include <time.h>
#  if HAVE_STRINGS_H
#    include <strings.h>
#  endif

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

  struct timeval timeout = {.tv_sec = tmo / 1000, .tv_usec = (tmo % 1000) * 1000};
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

#endif // defined(_WIN32)
