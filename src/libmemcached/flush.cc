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

static memcached_return_t memcached_flush_binary(Memcached *ptr, time_t expiration,
                                                 const bool reply) {
  protocol_binary_request_flush request = {};

  request.message.header.request.opcode = PROTOCOL_BINARY_CMD_FLUSH;
  request.message.header.request.extlen = 4;
  request.message.header.request.datatype = PROTOCOL_BINARY_RAW_BYTES;
  request.message.header.request.bodylen = htonl(request.message.header.request.extlen);
  request.message.body.expiration = htonl((uint32_t) expiration);

  memcached_return_t rc = MEMCACHED_SUCCESS;

  for (uint32_t x = 0; x < memcached_server_count(ptr); x++) {
    memcached_instance_st *instance = memcached_instance_fetch(ptr, x);
    initialize_binary_request(instance, request.message.header);

    if (reply) {
      request.message.header.request.opcode = PROTOCOL_BINARY_CMD_FLUSH;
    } else {
      request.message.header.request.opcode = PROTOCOL_BINARY_CMD_FLUSHQ;
    }

    libmemcached_io_vector_st vector[] = {{NULL, 0}, {request.bytes, sizeof(request.bytes)}};

    memcached_return_t rrc;
    if (memcached_failed(rrc = memcached_vdo(instance, vector, 2, true))) {
      if (instance->error_messages == NULL or instance->root->error_messages == NULL) {
        memcached_set_error(*instance, rrc, MEMCACHED_AT);
      }
      rc = MEMCACHED_SOME_ERRORS;
    }
  }

  for (uint32_t x = 0; x < memcached_server_count(ptr); x++) {
    memcached_instance_st *instance = memcached_instance_fetch(ptr, x);

    if (instance->response_count() > 0) {
      (void) memcached_response(instance, NULL, 0, NULL);
    }
  }

  return rc;
}

static memcached_return_t memcached_flush_textual(Memcached *ptr, time_t expiration,
                                                  const bool reply) {
  char buffer[MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH + 1 + 1];
  int send_length = 0;
  if (expiration) {
    send_length = snprintf(buffer, sizeof(buffer), "%lld", (long long) expiration);
  }

  if (size_t(send_length) >= sizeof(buffer) or send_length < 0) {
    return memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT,
                               memcached_literal_param("snprintf(MEMCACHED_DEFAULT_COMMAND_SIZE)"));
  }

  memcached_return_t rc = MEMCACHED_SUCCESS;
  for (uint32_t x = 0; x < memcached_server_count(ptr); x++) {
    memcached_instance_st *instance = memcached_instance_fetch(ptr, x);

    libmemcached_io_vector_st vector[] = {
        {NULL, 0},
        {memcached_literal_param("flush_all ")},
        {buffer, size_t(send_length)},
        {" noreply", reply ? 0 : memcached_literal_param_size(" noreply")},
        {memcached_literal_param("\r\n")}};

    memcached_return_t rrc = memcached_vdo(instance, vector, 5, true);
    if (memcached_success(rrc) and reply == true) {
      char response_buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
      rrc = memcached_response(instance, response_buffer, sizeof(response_buffer), NULL);
    }

    if (memcached_failed(rrc)) {
      // If an error has already been reported, then don't add to it
      if (instance->error_messages == NULL or instance->root->error_messages == NULL) {
        memcached_set_error(*instance, rrc, MEMCACHED_AT);
      }
      rc = MEMCACHED_SOME_ERRORS;
    }
  }

  return rc;
}

memcached_return_t memcached_flush(memcached_st *shell, time_t expiration) {
  Memcached *ptr = memcached2Memcached(shell);
  memcached_return_t rc;
  if (memcached_failed(rc = initialize_query(ptr, true))) {
    return rc;
  }

  bool reply = memcached_is_replying(ptr);

  LIBMEMCACHED_MEMCACHED_FLUSH_START();
  if (memcached_is_binary(ptr)) {
    rc = memcached_flush_binary(ptr, expiration, reply);
  } else {
    rc = memcached_flush_textual(ptr, expiration, reply);
  }
  LIBMEMCACHED_MEMCACHED_FLUSH_END();

  return rc;
}
