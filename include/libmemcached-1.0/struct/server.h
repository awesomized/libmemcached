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

#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif

#ifdef NI_MAXHOST
#  define MEMCACHED_NI_MAXHOST NI_MAXHOST
#else
#  define MEMCACHED_NI_MAXHOST 1025
#endif

#ifdef NI_MAXSERV
#  define MEMCACHED_NI_MAXSERV NI_MAXSERV
#else
#  define MEMCACHED_NI_MAXSERV 32
#endif

enum memcached_server_state_t {
  MEMCACHED_SERVER_STATE_NEW,      // fd == -1, no address lookup has been done
  MEMCACHED_SERVER_STATE_ADDRINFO, // ADDRRESS information has been gathered
  MEMCACHED_SERVER_STATE_IN_PROGRESS,
  MEMCACHED_SERVER_STATE_CONNECTED,
  MEMCACHED_SERVER_STATE_IN_TIMEOUT,
  MEMCACHED_SERVER_STATE_DISABLED
};

struct memcached_server_st {
  struct {
    bool is_allocated : 1;
    bool is_initialized : 1;
    bool is_shutting_down : 1;
    bool is_dead : 1;
  } options;
  uint32_t number_of_hosts;
  uint32_t cursor_active;
  in_port_t port;
  uint32_t io_bytes_sent; /* # bytes sent since last read */
  uint32_t request_id;
  uint32_t server_failure_counter;
  uint64_t server_failure_counter_query_id;
  uint32_t server_timeout_counter;
  uint64_t server_timeout_counter_query_id;
  uint32_t weight;
  uint32_t version;
  enum memcached_server_state_t state;
  struct {
    uint32_t read;
    uint32_t write;
    uint32_t timeouts;
    size_t _bytes_read;
  } io_wait_count;
  uint8_t major_version; // Default definition of UINT8_MAX means that it has not been set.
  uint8_t micro_version; // ditto, and note that this is the third, not second version bit
  uint8_t minor_version; // ditto
  memcached_connection_t type;
  time_t next_retry;
  struct memcached_st *root;
  uint64_t limit_maxbytes;
  struct memcached_error_t *error_messages;
  char hostname[MEMCACHED_NI_MAXHOST];
};
