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

#include "p9y/gettimeofday.hpp"

#include <cassert>

memcached_server_st *server_create_with(memcached_st *memc, memcached_server_st *self,
                                          const memcached_string_t &hostname, const in_port_t port,
                                          uint32_t weight, const memcached_connection_t type);

memcached_return_t memcached_server_add_parsed(memcached_st *ptr, const char *hostname,
                                               size_t hostname_length, in_port_t port,
                                               uint32_t weight);

void server_free(memcached_server_st *);

static inline bool memcached_is_valid_servername(const memcached_string_t &arg) {
  return (arg.c_str != NULL or arg.size == 0) and arg.size < MEMCACHED_NI_MAXHOST;
}

static inline bool memcached_is_valid_filename(const memcached_string_t &arg) {
  return arg.c_str != NULL and arg.size > 0 and arg.size < MEMCACHED_NI_MAXHOST;
}

void memcached_instance_free(memcached_instance_st *);

void set_last_disconnected_host(memcached_instance_st *self);

static inline void memcached_mark_server_for_timeout(memcached_instance_st *server) {
  if (server->state != MEMCACHED_SERVER_STATE_IN_TIMEOUT) {
    if (server->server_timeout_counter_query_id != server->root->query_id) {
      server->server_timeout_counter++;
      server->server_timeout_counter_query_id = server->root->query_id;
    }

    if (server->server_timeout_counter >= server->root->server_timeout_limit) {
      struct timeval next_time;
      if (gettimeofday(&next_time, NULL) == 0) {
        server->next_retry = next_time.tv_sec + server->root->retry_timeout;
      } else {
        server->next_retry = 1; // Setting the value to 1 causes the timeout to occur immediately
      }

      server->state = MEMCACHED_SERVER_STATE_IN_TIMEOUT;
      if (server->server_failure_counter_query_id != server->root->query_id) {
        server->server_failure_counter++;
        server->server_failure_counter_query_id = server->root->query_id;
      }
      set_last_disconnected_host(server);
    }
  }
}
