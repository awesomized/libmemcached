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

static memcached_return_t ascii_gat(memcached_instance_st *instance, const char *key,
                                    size_t key_length, time_t expiration) {
  char expiration_buffer[MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH + 1 + 1];
  int expiration_buffer_length = snprintf(expiration_buffer, sizeof(expiration_buffer), "%lld",
                                          (long long) expiration);
  if (size_t(expiration_buffer_length) >= sizeof(expiration_buffer) + 1
      or expiration_buffer_length < 0)
  {
    return memcached_set_error(
        *instance, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT,
        memcached_literal_param("snprintf(MEMCACHED_MAXIMUM_INTEGER_DISPLAY_LENGTH)"));
  }

  /* Use "gats" when CAS support is requested so the VALUE response includes the CAS token */
  const char *gat_command;
  uint8_t gat_command_length;
  if (instance->root->flags.support_cas) {
    gat_command = "gats ";
    gat_command_length = 5;
  } else {
    gat_command = "gat ";
    gat_command_length = 4;
  }

  libmemcached_io_vector_st vector[] = {
      {NULL, 0},
      {gat_command, gat_command_length},
      {expiration_buffer, size_t(expiration_buffer_length)},
      {memcached_literal_param(" ")},
      {memcached_array_string(instance->root->_namespace),
       memcached_array_size(instance->root->_namespace)},
      {key, key_length},
      {memcached_literal_param("\r\n")}};

  memcached_return_t rc;
  if (memcached_failed(rc = memcached_vdo(instance, vector, 7, true))) {
    return memcached_set_error(*instance, MEMCACHED_WRITE_FAILURE, MEMCACHED_AT);
  }

  return rc;
}

static memcached_return_t binary_gat(memcached_instance_st *instance, const char *key,
                                     size_t key_length, time_t expiration) {
  protocol_binary_request_gat request = {};

  initialize_binary_request(instance, request.message.header);

  /* GATK returns the key in the response, matching how GETK is used for binary get */
  request.message.header.request.opcode = PROTOCOL_BINARY_CMD_GATK;
  request.message.header.request.extlen = 4;
  request.message.header.request.keylen =
      htons((uint16_t)(key_length + memcached_array_size(instance->root->_namespace)));
  request.message.header.request.datatype = PROTOCOL_BINARY_RAW_BYTES;
  request.message.header.request.bodylen =
      htonl((uint32_t)(key_length + memcached_array_size(instance->root->_namespace)
                       + request.message.header.request.extlen));
  request.message.body.expiration = htonl((uint32_t) expiration);

  libmemcached_io_vector_st vector[] = {
      {NULL, 0},
      {request.bytes, sizeof(request.bytes)},
      {memcached_array_string(instance->root->_namespace),
       memcached_array_size(instance->root->_namespace)},
      {key, key_length}};

  memcached_return_t rc;
  if (memcached_failed(rc = memcached_vdo(instance, vector, 4, true))) {
    return memcached_set_error(*instance, MEMCACHED_WRITE_FAILURE, MEMCACHED_AT);
  }

  return rc;
}

char *memcached_gat(memcached_st *ptr, const char *key, size_t key_length, time_t expiration,
                    size_t *value_length, uint32_t *flags, memcached_return_t *error) {
  return memcached_gat_by_key(ptr, NULL, 0, key, key_length, expiration, value_length, flags,
                               error);
}

char *memcached_gat_by_key(memcached_st *shell, const char *group_key, size_t group_key_length,
                           const char *key, size_t key_length, time_t expiration,
                           size_t *value_length, uint32_t *flags, memcached_return_t *error) {
  Memcached *ptr = memcached2Memcached(shell);
  memcached_return_t unused;
  if (error == NULL) {
    error = &unused;
  }

  memcached_return_t rc;
  if (memcached_failed(rc = initialize_query(ptr, true))) {
    *error = rc;
    if (value_length) {
      *value_length = 0;
    }
    return NULL;
  }

  if (memcached_is_udp(ptr)) {
    *error = memcached_set_error(*ptr, MEMCACHED_NOT_SUPPORTED, MEMCACHED_AT);
    if (value_length) {
      *value_length = 0;
    }
    return NULL;
  }

  if (memcached_failed(rc = memcached_key_test(*ptr, (const char **) &key, &key_length, 1))) {
    *error = memcached_set_error(*ptr, rc, MEMCACHED_AT);
    if (value_length) {
      *value_length = 0;
    }
    return NULL;
  }

  uint32_t server_key;
  if (group_key and group_key_length) {
    server_key = memcached_generate_hash_with_redistribution(ptr, group_key, group_key_length);
  } else {
    server_key = memcached_generate_hash_with_redistribution(ptr, key, key_length);
  }
  memcached_instance_st *instance = memcached_instance_fetch(ptr, server_key);

  if (ptr->flags.binary_protocol) {
    rc = binary_gat(instance, key, key_length, expiration);
  } else {
    rc = ascii_gat(instance, key, key_length, expiration);
  }

  if (memcached_failed(rc)) {
    *error = rc;
    if (value_length) {
      *value_length = 0;
    }
    return NULL;
  }

  char *value = memcached_fetch(ptr, NULL, NULL, value_length, flags, error);

  /* Normalize END (no key found) to NOTFOUND, matching memcached_get behavior */
  if (*error == MEMCACHED_END) {
    *error = MEMCACHED_NOTFOUND;
  }

  return value;
}
