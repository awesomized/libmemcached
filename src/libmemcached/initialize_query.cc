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

#include "libmemcached/common.h"

memcached_return_t initialize_query(Memcached *self, bool increment_query_id) {
  if (self == NULL) {
    return MEMCACHED_INVALID_ARGUMENTS;
  }

  if (increment_query_id) {
    self->query_id++;
  }

  if (self->state.is_time_for_rebuild) {
    memcached_reset(self);
  }

  if (memcached_server_count(self) == 0) {
    return memcached_set_error(*self, MEMCACHED_NO_SERVERS, MEMCACHED_AT);
  }

  memcached_error_free(*self);
  memcached_result_reset(&self->result);

  return MEMCACHED_SUCCESS;
}

memcached_return_t initialize_const_query(const Memcached *self) {
  if (self == NULL) {
    return MEMCACHED_INVALID_ARGUMENTS;
  }

  if (memcached_server_count(self) == 0) {
    return MEMCACHED_NO_SERVERS;
  }

  return MEMCACHED_SUCCESS;
}
