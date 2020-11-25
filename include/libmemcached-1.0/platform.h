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

#include "libmemcached-1.0/configure.h"

#if defined(_WIN32)
#  include <winsock2.h>
#  include <ws2tcpip.h>

#  ifndef HAVE_IN_PORT_T
typedef int in_port_t;
#    define HAVE_IN_PORT_T 1
#  endif

typedef SOCKET memcached_socket_t;

#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <sys/un.h>
#  include <netinet/tcp.h>

typedef int memcached_socket_t;

#endif /* _WIN32 */
