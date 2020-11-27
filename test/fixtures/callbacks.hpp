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

#include "test/lib/common.hpp"
#include "libmemcached/common.h"
#include "libmemcachedutil/common.h"

static inline memcached_return_t ping_callback(const memcached_st *,
                                               const memcached_instance_st *instance, void *) {
  memcached_return_t rc;

  REQUIRE(libmemcached_util_ping(memcached_server_name(instance), memcached_server_port(instance),
                                 &rc));
  REQUIRE(rc == MEMCACHED_SUCCESS);
  return MEMCACHED_SUCCESS;
}

static inline memcached_return_t ping2_callback(const memcached_st *,
                                                const memcached_instance_st *instance, void *) {
  memcached_return_t rc;

  REQUIRE(libmemcached_util_ping2(memcached_server_name(instance), memcached_server_port(instance),
                                  "memcached", "memcached", &rc));
  REQUIRE(rc == MEMCACHED_SUCCESS);
  return MEMCACHED_SUCCESS;
}

static inline memcached_return_t callback_counter(const memcached_st *, memcached_result_st *,
                                                  void *context) {
  auto *counter = reinterpret_cast<size_t *>(context);
  *counter = *counter + 1;

  return MEMCACHED_SUCCESS;
}

static inline memcached_return_t delete_trigger(memcached_st *, const char *, size_t) {
  return MEMCACHED_SUCCESS;
}

static inline memcached_return_t get_failure(memcached_st *, char *, size_t,
                                                      memcached_result_st *result) {
  return memcached_result_set_value(result, S("updated by read through trigger"));
}

static inline memcached_return_t clone_callback(memcached_st *, memcached_st *) {
  return MEMCACHED_SUCCESS;
}

static inline memcached_return_t cleanup_callback(memcached_st *) {
  return MEMCACHED_SUCCESS;
}

static inline memcached_return_t
server_sort_callback(const memcached_st *, const memcached_instance_st *server, void *context) {
  if (context) {
    auto bigger = reinterpret_cast<size_t *>(context);
    REQUIRE(*bigger <= memcached_server_port(server));
    *bigger = memcached_server_port(server);
  }
  return MEMCACHED_SUCCESS;
}
