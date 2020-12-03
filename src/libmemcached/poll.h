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

#if defined HAVE_SYS_POLL_H
#  include <sys/poll.h>
#elif defined HAVE_POLL_H
#  include <poll.h>
#elif defined _WIN32
#  include "windows.hpp"
#  define poll WSAPoll
typedef int nfds_t;
#elif !defined _MSC_VER

#  ifdef __cplusplus
extern "C" {
#  endif

typedef struct pollfd {
#  if defined(_WIN32)
  SOCKET fd;
#  else
  int fd;
#  endif
  short events;
  short revents;
} pollfd_t;

typedef int nfds_t;

#  define POLLIN   0x0001
#  define POLLOUT  0x0004
#  define POLLERR  0x0008
#  define POLLHUP  0x010 /* Hung up.  */
#  define POLLNVAL 0x020 /* Invalid polling request.  */

int poll(struct pollfd fds[], nfds_t nfds, int tmo);

#  ifdef __cplusplus
}
#  endif

#endif // defined(_WIN32)
