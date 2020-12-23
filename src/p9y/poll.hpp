#pragma once

#include "libmemcached-1.0/platform.h"

#if defined HAVE_POLL_H
#  include <poll.h>
#elif defined HAVE_SYS_POLL_H
#  include <sys/poll.h>
#elif defined _WIN32
#  define poll WSAPoll
typedef int nfds_t;
#else

#  define P9Y_NEED_POLL

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

#endif
