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

static inline bool memcached_success(memcached_return_t rc) {
  return (rc == MEMCACHED_BUFFERED || rc == MEMCACHED_DELETED || rc == MEMCACHED_END
          || rc == MEMCACHED_ITEM || rc == MEMCACHED_STAT || rc == MEMCACHED_STORED
          || rc == MEMCACHED_SUCCESS || rc == MEMCACHED_VALUE);
}

static inline bool memcached_failed(memcached_return_t rc) {
  return (rc != MEMCACHED_AUTH_CONTINUE && rc != MEMCACHED_BUFFERED && rc != MEMCACHED_DELETED
          && rc != MEMCACHED_END && rc != MEMCACHED_ITEM && rc != MEMCACHED_STAT
          && rc != MEMCACHED_STORED && rc != MEMCACHED_SUCCESS && rc != MEMCACHED_VALUE);
}

static inline bool memcached_fatal(memcached_return_t rc) {
  return (rc != MEMCACHED_AUTH_CONTINUE && rc != MEMCACHED_BUFFERED && rc != MEMCACHED_CLIENT_ERROR
          && rc != MEMCACHED_DATA_EXISTS && rc != MEMCACHED_DELETED && rc != MEMCACHED_E2BIG
          && rc != MEMCACHED_END && rc != MEMCACHED_ITEM && rc != MEMCACHED_ERROR
          && rc != MEMCACHED_NOTFOUND && rc != MEMCACHED_NOTSTORED
          && rc != MEMCACHED_SERVER_MEMORY_ALLOCATION_FAILURE && rc != MEMCACHED_STAT
          && rc != MEMCACHED_STORED && rc != MEMCACHED_SUCCESS && rc != MEMCACHED_VALUE);
}

#define memcached_continue(__memcached_return_t) ((__memcached_return_t) == MEMCACHED_IN_PROGRESS)
