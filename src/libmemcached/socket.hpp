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

/* To hide the platform differences between MS Windows and Unix, I am
 * going to use the Microsoft way and #define the Microsoft-specific
 * functions to the unix way. Microsoft use a separate subsystem for sockets,
 * but Unix normally just use a filedescriptor on the same functions. It is
 * a lot easier to map back to the unix way with macros than going the other
 * way without side effect ;-)
 */
#if defined(WIN32) || defined(__MINGW32__)
#  include "win32/wrappers.h"
#  define get_socket_errno() WSAGetLastError()
#else
#  include <unistd.h>
#  define INVALID_SOCKET     -1
#  define SOCKET_ERROR       -1
#  define closesocket(a)     close(a)
#  define get_socket_errno() errno
#endif

#ifdef __cplusplus
static inline void memcached_close_socket(memcached_socket_t &socket_fd) {
  closesocket(socket_fd);
  socket_fd = INVALID_SOCKET;
}
#endif

#ifndef HAVE_MSG_NOSIGNAL
#  define MSG_NOSIGNAL 0
#endif

#ifndef HAVE_MSG_DONTWAIT
#  define MSG_DONTWAIT 0
#endif

#ifndef HAVE_MSG_MORE
#  define MSG_MORE 0
#endif
