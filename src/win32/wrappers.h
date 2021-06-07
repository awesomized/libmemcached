/* LibMemcached
 * Copyright (C) 2010 Brian Aker, Trond Norbye
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: "Implementation" of the function we don't have on windows
 *          to avoid a bunch of ifdefs in the rest of the code
 *
 */
#pragma once 

#include <inttypes.h>

/*
 * One of the Windows headers define interface as a macro, but that
 * is causing problems with the member named "interface" in some of the
 * structs.
 */
#undef interface

#undef malloc
#undef realloc


/*
 * WinSock use a separate range for error codes. Let's just map to the
 * WinSock ones.
 */
#ifndef EADDRINUSE
# define EADDRINUSE WSAEADDRINUSE
#endif

#ifndef EWOULDBLOCK
# define EWOULDBLOCK WSAEWOULDBLOCK
#endif

#ifndef EINPROGRESS
# define EINPROGRESS WSAEINPROGRESS
#endif

#ifndef EALREADY
# define EALREADY WSAEALREADY
#endif

#ifndef EISCONN
# define EISCONN WSAEISCONN
#endif

#ifndef ENOTCONN
# define ENOTCONN WSAENOTCONN
#endif

#ifndef ENOBUFS
# define ENOBUFS WSAENOBUFS
#endif

#ifndef SHUT_RDWR
# define SHUT_RDWR SD_BOTH
#endif
#ifndef SHUT_WR
# define SHUT_WR SD_SEND
#endif
#ifndef SHUT_RD
# define SHUT_RD SD_RECEIVE
#endif

/* EAI_SYSTEM isn't defined anywhere... just set it to... 11? */
#ifndef EAI_SYSTEM
# define EAI_SYSTEM 11
#endif

/* Best effort mapping of functions to alternative functions */
#define index(a,b) strchr(a,b)
#define rindex(a,b) strrchr(a,b)
#define random() rand()
#define srandom(a) while (false) {}
#define kill(a, b) while (false) {}
#define fork() (-1)
#define waitpid(a,b,c) (-1)
#define fnmatch(a,b,c) (-1)
#define sleep(a) Sleep(a*1000)

#ifdef __cplusplus
#  include <chrono>
static inline int gettimeofday(struct timeval* tp, struct timezone* tzp) {
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
#endif
