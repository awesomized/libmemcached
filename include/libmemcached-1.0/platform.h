/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#pragma once

#include "libmemcached-1.0/configure.h"

/* This seems to be required for older compilers @note
 * http://stackoverflow.com/questions/8132399/how-to-printf-uint64-t  */
#ifndef __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS
#endif

#ifdef __cplusplus
#  include <cinttypes>
#  include <cstddef>
#  include <cstdlib>
#else
#  include <inttypes.h>
#  include <stddef.h>
#  include <stdlib.h>
#  include <stdbool.h>
#endif

#include <sys/types.h>

#if defined HAVE_NETDB_H
# include <netdb.h>
#endif

#if !defined HAVE_IN_PORT_T
typedef int in_port_t;
#endif

#if !defined HAVE_PID_T
typedef int pid_t;
#endif

#ifndef HAVE_SSIZE_T
typedef long int ssize_t;
#endif

#if defined _WIN32
# define WINVER 0x0600
# define _WIN32_WINNT 0x0600
# include <winsock2.h>
# include <ws2tcpip.h>
typedef SOCKET memcached_socket_t;
#else
typedef int memcached_socket_t;
#endif // _WIN32
