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

const char *memcached_lib_version(void) {
  return LIBMEMCACHED_VERSION_STRING;
}

static inline memcached_return_t memcached_version_textual(Memcached *memc) {
  libmemcached_io_vector_st vector[] = {
      {memcached_literal_param("version\r\n")},
  };

  uint32_t success = 0;
  bool errors_happened = false;
  for (uint32_t x = 0; x < memcached_server_count(memc); x++) {
    memcached_instance_st *instance = memcached_instance_fetch(memc, x);

    // Optimization, we only fetch version once.
    if (instance->major_version != UINT8_MAX) {
      continue;
    }

    memcached_return_t rrc;
    if (memcached_failed(rrc = memcached_vdo(instance, vector, 1, true))) {
      errors_happened = true;
      (void) memcached_set_error(*instance, rrc, MEMCACHED_AT);
      continue;
    }
    success++;
  }

  if (success) {
    // Collect the returned items
    memcached_instance_st *instance;
    memcached_return_t readable_error;
    while ((instance = memcached_io_get_readable_server(memc, readable_error))) {
      memcached_return_t rrc = memcached_response(instance, NULL);
      if (memcached_failed(rrc)) {
        errors_happened = true;
      }
    }
  }

  return errors_happened ? MEMCACHED_SOME_ERRORS : MEMCACHED_SUCCESS;
}

static inline memcached_return_t memcached_version_binary(Memcached *memc) {
  protocol_binary_request_version request = {};

  request.message.header.request.opcode = PROTOCOL_BINARY_CMD_VERSION;
  request.message.header.request.datatype = PROTOCOL_BINARY_RAW_BYTES;

  libmemcached_io_vector_st vector[] = {{request.bytes, sizeof(request.bytes)}};

  uint32_t success = 0;
  bool errors_happened = false;
  for (uint32_t x = 0; x < memcached_server_count(memc); x++) {
    memcached_instance_st *instance = memcached_instance_fetch(memc, x);

    initialize_binary_request(instance, request.message.header);

    if (instance->major_version != UINT8_MAX) {
      continue;
    }

    memcached_return_t rrc = memcached_vdo(instance, vector, 1, true);
    if (memcached_failed(rrc)) {
      errors_happened = true;
      continue;
    }

    success++;
  }

  if (success) {
    // Collect the returned items
    memcached_instance_st *instance;
    memcached_return_t readable_error;
    while ((instance = memcached_io_get_readable_server(memc, readable_error))) {
      char buffer[32];
      memcached_return_t rrc = memcached_response(instance, buffer, sizeof(buffer), NULL);
      if (memcached_failed(rrc)) {
        errors_happened = true;
      }
    }
  }

  return errors_happened ? MEMCACHED_SOME_ERRORS : MEMCACHED_SUCCESS;
}

static inline void version_ascii_instance(memcached_instance_st *instance) {
  if (instance->major_version != UINT8_MAX) {
    libmemcached_io_vector_st vector[] = {
        {memcached_literal_param("version\r\n")},
    };

    (void) memcached_vdo(instance, vector, 1, false);
  }
}

static inline void version_binary_instance(memcached_instance_st *instance) {
  if (instance->major_version != UINT8_MAX) {
    protocol_binary_request_version request = {};

    request.message.header.request.opcode = PROTOCOL_BINARY_CMD_VERSION;
    request.message.header.request.datatype = PROTOCOL_BINARY_RAW_BYTES;

    libmemcached_io_vector_st vector[] = {{request.bytes, sizeof(request.bytes)}};

    initialize_binary_request(instance, request.message.header);

    (void) memcached_vdo(instance, vector, 1, false);
  }
}

void memcached_version_instance(memcached_instance_st *instance) {
  if (instance) {
    if (memcached_has_root(instance)) {
      if (memcached_is_fetching_version(instance->root)) {
        if (memcached_is_udp(instance->root) == false) {
          if (memcached_is_binary(instance->root)) {
            version_binary_instance(instance);
            return;
          }

          version_ascii_instance(instance);
        }
      }
    }
  }
}

int8_t memcached_version_instance_cmp(memcached_instance_st *instance, uint8_t maj, uint8_t min,
                                      uint8_t mic) {
  if (!instance || memcached_server_major_version(instance) == UINT8_MAX) {
    return INT8_MIN;
  } else {
    uint32_t sv, cv;

    sv = memcached_server_micro_version(instance) | memcached_server_minor_version(instance) << 8
        | memcached_server_major_version(instance) << 16;
    cv = mic | min << 8 | maj << 16;
    if (sv < cv) {
      return -1;
    }
    return sv != cv;
  }
}

memcached_return_t memcached_version(memcached_st *shell) {
  Memcached *memc = memcached2Memcached(shell);
  if (memc) {
    memcached_return_t rc;
    if (memcached_failed(rc = initialize_query(memc, true))) {
      return rc;
    }

    if (memcached_is_udp(memc)) {
      return MEMCACHED_NOT_SUPPORTED;
    }

    if (memcached_is_binary(memc)) {
      return memcached_version_binary(memc);
    }

    return memcached_version_textual(memc);
  }

  return MEMCACHED_INVALID_ARGUMENTS;
}
