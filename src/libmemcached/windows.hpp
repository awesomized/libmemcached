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

#ifdef __cplusplus
#  include <cerrno>
#else
#  include <errno.h>
#endif

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0501
#endif

#ifdef __MINGW32__
#  if (_WIN32_WINNT >= 0x0501)
#  else
#    undef _WIN32_WINNT
#    define _WIN32_WINNT 0x0501
#  endif /* _WIN32_WINNT >= 0x0501 */
#endif   /* __MINGW32__ */

#if defined(HAVE_WINSOCK2_H) && HAVE_WINSOCK2_H
#  include <winsock2.h>
#endif

#if defined(HAVE_WS2TCPIP_H) && HAVE_WS2TCPIP_H
#  include <ws2tcpip.h>
#endif

#if defined(HAVE_IO_H) && HAVE_IO_H
#  include <io.h>
#endif

struct sockaddr_un {
  short int sun_family;
  char sun_path[108];
};

static inline int translate_windows_error() {
  int local_errno = WSAGetLastError();

  switch (local_errno) {
  case WSAEINVAL: local_errno = EINPROGRESS; break;
  case WSAEALREADY:
  case WSAEWOULDBLOCK: local_errno = EAGAIN; break;

  case WSAECONNREFUSED: local_errno = ECONNREFUSED; break;

  case WSAENETUNREACH: local_errno = ENETUNREACH; break;

  case WSAETIMEDOUT: local_errno = ETIMEDOUT; break;

  case WSAECONNRESET: local_errno = ECONNRESET; break;

  case WSAEADDRINUSE: local_errno = EADDRINUSE; break;

  case WSAEOPNOTSUPP: local_errno = EOPNOTSUPP; break;

  case WSAENOPROTOOPT: local_errno = ENOPROTOOPT; break;

  default: break;
  }

  return local_errno;
}
