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

struct context_st {
  size_t length;
  const char *buffer;
};

static memcached_return_t _set_verbosity(const Memcached *, const memcached_instance_st *server,
                                         void *context) {
  libmemcached_io_vector_st *vector = (libmemcached_io_vector_st *) context;

  Memcached local_memc;
  Memcached *memc_ptr = memcached_create(&local_memc);

  memcached_return_t rc =
      memcached_server_add(memc_ptr, memcached_server_name(server), memcached_server_port(server));

  if (rc == MEMCACHED_SUCCESS) {
    memcached_instance_st *instance = memcached_instance_fetch(memc_ptr, 0);

    rc = memcached_vdo(instance, vector, 2, true);

    if (rc == MEMCACHED_SUCCESS) {
      char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
      rc = memcached_response(instance, buffer, sizeof(buffer), NULL);
    }
  }

  memcached_free(memc_ptr);

  return rc;
}

memcached_return_t memcached_verbosity(memcached_st *shell, uint32_t verbosity) {
  Memcached *ptr = memcached2Memcached(shell);
  memcached_return_t rc;
  if (memcached_failed(rc = initialize_query(ptr, false))) {
    return rc;
  }

  memcached_server_fn callbacks[1];

  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  int send_length = snprintf(buffer, sizeof(buffer), "verbosity %u\r\n", verbosity);
  if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE or send_length < 0) {
    return memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT,
                               memcached_literal_param("snprintf(MEMCACHED_DEFAULT_COMMAND_SIZE)"));
  }

  libmemcached_io_vector_st vector[] = {
      {NULL, 0},
      {buffer, size_t(send_length)},
  };

  callbacks[0] = _set_verbosity;

  return memcached_server_cursor(ptr, callbacks, vector, 1);
}
