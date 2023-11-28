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

namespace {
memcached_return_t send_quit_message(memcached_instance_st *instance) {
  memcached_return_t rc;
  if (instance->root->flags.binary_protocol) {
    protocol_binary_request_quit request = {}; // = {.bytes= {0}};

    initialize_binary_request(instance, request.message.header);

    request.message.header.request.opcode = PROTOCOL_BINARY_CMD_QUIT;
    request.message.header.request.datatype = PROTOCOL_BINARY_RAW_BYTES;

    libmemcached_io_vector_st vector[] = {{request.bytes, sizeof(request.bytes)}};

    rc = memcached_vdo(instance, vector, 1, true);
  } else {
    libmemcached_io_vector_st vector[] = {{memcached_literal_param("quit\r\n")}};

    rc = memcached_vdo(instance, vector, 1, true);
  }

  return rc;
}

void drain_instance(memcached_instance_st *instance) {
  /* read until socket is closed, or there is an error
   * closing the socket before all data is read
   * results in server throwing away all data which is
   * not read
   *
   * In .40 we began to only do this if we had been doing buffered
   * requests of had replication enabled.
   */
  if (instance->root->flags.buffer_requests or instance->root->number_of_replicas) {
    memcached_io_slurp(instance);
  }

  /*
   * memcached_io_read may call memcached_quit_server with io_death if
   * it encounters problems, but we don't care about those occurences.
   * The intention of that loop is to drain the data sent from the
   * server to ensure that the server processed all of the data we
   * sent to the server.
   */
  instance->server_failure_counter = 0;
  instance->server_timeout_counter = 0;
}
} // namespace

/*
  This closes all connections (forces flush of input as well).

  Maybe add a host specific, or key specific version?

  The reason we send "quit" is that in case we have buffered IO, this
  will force data to be completed.
*/

void memcached_quit_server(memcached_instance_st *instance, bool io_death) {
  if (instance->valid()) {
    if (io_death == false and memcached_is_udp(instance->root) == false
        and instance->is_shutting_down() == false)
    {
      send_quit_message(instance);

      instance->start_close_socket();
      drain_instance(instance);
    }
  }

  instance->close_socket();

  if (io_death and memcached_is_udp(instance->root)) {
    /*
       If using UDP, we should stop using the server briefly on every IO
       failure. If using TCP, it may be that the connection went down a
       short while ago (e.g. the server failed) and we've only just
       noticed, so we should only set the retry timeout on a connect
       failure (which doesn't call this method).
       */
    memcached_mark_server_for_timeout(instance);
  }
}

void send_quit(Memcached *memc) {
  for (uint32_t x = 0; x < memcached_server_count(memc); x++) {
    memcached_instance_st *instance = memcached_instance_fetch(memc, x);

    memcached_quit_server(instance, false);
  }
}

void memcached_quit(memcached_st *shell) {
  Memcached *memc = memcached2Memcached(shell);
  memcached_return_t rc;
  if (memcached_failed(rc = initialize_query(memc, true))) {
    return;
  }

  send_quit(memc);
}