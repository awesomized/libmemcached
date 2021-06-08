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

enum memcached_return_t {
  MEMCACHED_SUCCESS,
  MEMCACHED_FAILURE,
  MEMCACHED_HOST_LOOKUP_FAILURE, // getaddrinfo() and getnameinfo() only
  MEMCACHED_CONNECTION_FAILURE,
  MEMCACHED_CONNECTION_BIND_FAILURE, // DEPRECATED, see MEMCACHED_HOST_LOOKUP_FAILURE
  MEMCACHED_WRITE_FAILURE,
  MEMCACHED_READ_FAILURE,
  MEMCACHED_UNKNOWN_READ_FAILURE,
  MEMCACHED_PROTOCOL_ERROR,
  MEMCACHED_CLIENT_ERROR,
  MEMCACHED_SERVER_ERROR, // Server returns "SERVER_ERROR"
  MEMCACHED_ERROR,        // Server returns "ERROR"
  MEMCACHED_DATA_EXISTS,
  MEMCACHED_DATA_DOES_NOT_EXIST,
  MEMCACHED_NOTSTORED,
  MEMCACHED_STORED,
  MEMCACHED_NOTFOUND,
  MEMCACHED_MEMORY_ALLOCATION_FAILURE,
  MEMCACHED_PARTIAL_READ,
  MEMCACHED_SOME_ERRORS,
  MEMCACHED_NO_SERVERS,
  MEMCACHED_END,
  MEMCACHED_DELETED,
  MEMCACHED_VALUE,
  MEMCACHED_STAT,
  MEMCACHED_ITEM,
  MEMCACHED_ERRNO,
  MEMCACHED_FAIL_UNIX_SOCKET, // DEPRECATED
  MEMCACHED_NOT_SUPPORTED,
  MEMCACHED_NO_KEY_PROVIDED, /* Deprecated. Use MEMCACHED_BAD_KEY_PROVIDED! */
  MEMCACHED_FETCH_NOTFINISHED,
  MEMCACHED_TIMEOUT,
  MEMCACHED_BUFFERED,
  MEMCACHED_BAD_KEY_PROVIDED,
  MEMCACHED_INVALID_HOST_PROTOCOL,
  MEMCACHED_SERVER_MARKED_DEAD,
  MEMCACHED_UNKNOWN_STAT_KEY,
  MEMCACHED_E2BIG,
  MEMCACHED_INVALID_ARGUMENTS,
  MEMCACHED_KEY_TOO_BIG,
  MEMCACHED_AUTH_PROBLEM,
  MEMCACHED_AUTH_FAILURE,
  MEMCACHED_AUTH_CONTINUE,
  MEMCACHED_PARSE_ERROR,
  MEMCACHED_PARSE_USER_ERROR,
  MEMCACHED_DEPRECATED,
  MEMCACHED_IN_PROGRESS,
  MEMCACHED_SERVER_TEMPORARILY_DISABLED,
  MEMCACHED_SERVER_MEMORY_ALLOCATION_FAILURE,
  MEMCACHED_MAXIMUM_RETURN, /* Always add new error code before */
  MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE = MEMCACHED_ERROR
};

#ifndef __cplusplus
typedef enum memcached_return_t memcached_return_t;
#endif
