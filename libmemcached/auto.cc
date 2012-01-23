/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <libmemcached/common.h>

static memcached_return_t text_incr_decr(memcached_server_write_instance_st instance,
                                         const bool is_incr,
                                         const char *key, size_t key_length,
                                         const uint64_t offset,
                                         const bool reply,
                                         uint64_t& numeric_value)
{
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  int send_length= snprintf(buffer, sizeof(buffer), " %" PRIu64, offset);
  if (size_t(send_length) >= sizeof(buffer) or send_length < 0)
  {
    return memcached_set_error(*instance, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT, 
                               memcached_literal_param("snprintf(MEMCACHED_DEFAULT_COMMAND_SIZE)"));
  }

  libmemcached_io_vector_st vector[]=
  {
    { NULL, 0 },
    { memcached_literal_param("incr ") },
    { memcached_array_string(instance->root->_namespace), memcached_array_size(instance->root->_namespace) },
    { key, key_length },
    { buffer, send_length },
    { " noreply", reply ? 0 : memcached_literal_param_size(" noreply") },
    { memcached_literal_param("\r\n") }
  };

  if (is_incr == false)
  {
    vector[1].buffer= "decr ";
  }

  memcached_return_t rc= memcached_vdo(instance, vector, 7, true);

  if (reply == false)
  {
    return MEMCACHED_SUCCESS;
  }

  if (memcached_failed(rc))
  {
    numeric_value= UINT64_MAX;
    return rc;
  }

  rc= memcached_response(instance, buffer, sizeof(buffer), NULL, numeric_value);

  return memcached_set_error(*instance, rc, MEMCACHED_AT);
}

static memcached_return_t binary_incr_decr(memcached_server_write_instance_st instance,
                                           protocol_binary_command cmd,
                                           const char *key, const size_t key_length,
                                           const uint64_t offset,
                                           const uint64_t initial,
                                           const uint32_t expiration,
                                           const bool reply,
                                           uint64_t *value)
{
  if (reply == false)
  {
    if(cmd == PROTOCOL_BINARY_CMD_DECREMENT)
    {
      cmd= PROTOCOL_BINARY_CMD_DECREMENTQ;
    }

    if(cmd == PROTOCOL_BINARY_CMD_INCREMENT)
    {
      cmd= PROTOCOL_BINARY_CMD_INCREMENTQ;
    }
  }
  protocol_binary_request_incr request= {}; // = {.bytes= {0}};

  request.message.header.request.magic= PROTOCOL_BINARY_REQ;
  request.message.header.request.opcode= cmd;
  request.message.header.request.keylen= htons((uint16_t)(key_length + memcached_array_size(instance->root->_namespace)));
  request.message.header.request.extlen= 20;
  request.message.header.request.datatype= PROTOCOL_BINARY_RAW_BYTES;
  request.message.header.request.bodylen= htonl((uint32_t)(key_length + memcached_array_size(instance->root->_namespace) +request.message.header.request.extlen));
  request.message.body.delta= memcached_htonll(offset);
  request.message.body.initial= memcached_htonll(initial);
  request.message.body.expiration= htonl((uint32_t) expiration);

  libmemcached_io_vector_st vector[]=
  {
    { NULL, 0 },
    { request.bytes, sizeof(request.bytes) },
    { memcached_array_string(instance->root->_namespace), memcached_array_size(instance->root->_namespace) },
    { key, key_length }
  };

  memcached_return_t rc;
  if (memcached_failed(rc= memcached_vdo(instance, vector, 4, true)))
  {
    memcached_io_reset(instance);
    return MEMCACHED_WRITE_FAILURE;
  }

  if (reply == false)
  {
    return MEMCACHED_SUCCESS;
  }

  return memcached_response(instance, (char*)value, sizeof(*value), NULL);
}

memcached_return_t memcached_increment(memcached_st *ptr,
                                       const char *key, size_t key_length,
                                       uint32_t offset,
                                       uint64_t *value)
{
  return memcached_increment_by_key(ptr, key, key_length, key, key_length, offset, value);
}

memcached_return_t memcached_decrement(memcached_st *ptr,
                                       const char *key, size_t key_length,
                                       uint32_t offset,
                                       uint64_t *value)
{
  return memcached_decrement_by_key(ptr, key, key_length, key, key_length, offset, value);
}

memcached_return_t memcached_increment_by_key(memcached_st *ptr,
                                              const char *group_key, size_t group_key_length,
                                              const char *key, size_t key_length,
                                              uint64_t offset,
                                              uint64_t *value)
{
  memcached_return_t rc;
  uint64_t local_value;
  if (value == NULL)
  {
    value= &local_value;
  }

  if (memcached_failed(rc= initialize_query(ptr, true)))
  {
    return rc;
  }

  if (memcached_failed(rc= memcached_key_test(*ptr, (const char **)&key, &key_length, 1)))
  {
    return rc;
  }

  uint32_t server_key= memcached_generate_hash_with_redistribution(ptr, group_key, group_key_length);
  memcached_server_write_instance_st instance= memcached_server_instance_fetch(ptr, server_key);

  bool reply= memcached_is_replying(instance->root);

  LIBMEMCACHED_MEMCACHED_INCREMENT_START();
  if (memcached_is_binary(ptr))
  {
    rc= binary_incr_decr(instance, PROTOCOL_BINARY_CMD_INCREMENT,
                         key, key_length,
                         uint64_t(offset), 0, MEMCACHED_EXPIRATION_NOT_ADD,
                         reply,
                         value);
  }
  else
  {
    rc= text_incr_decr(instance, true, key, key_length, offset, reply, *value);
  }

  LIBMEMCACHED_MEMCACHED_INCREMENT_END();

  return rc;
}

memcached_return_t memcached_decrement_by_key(memcached_st *ptr,
                                              const char *group_key, size_t group_key_length,
                                              const char *key, size_t key_length,
                                              uint64_t offset,
                                              uint64_t *value)
{
  uint64_t local_value;
  if (value == NULL)
  {
    value= &local_value;
  }

  memcached_return_t rc;
  if (memcached_failed(rc= initialize_query(ptr, true)))
  {
    return rc;
  }

  if (memcached_failed(rc= memcached_key_test(*ptr, (const char **)&key, &key_length, 1)))
  {
    return rc;
  }


  uint32_t server_key= memcached_generate_hash_with_redistribution(ptr, group_key, group_key_length);
  memcached_server_write_instance_st instance= memcached_server_instance_fetch(ptr, server_key);

  bool reply= memcached_is_replying(instance->root);

  LIBMEMCACHED_MEMCACHED_DECREMENT_START();
  if (memcached_is_binary(ptr))
  {
    rc= binary_incr_decr(instance, PROTOCOL_BINARY_CMD_DECREMENT,
                         key, key_length,
                         offset, 0, MEMCACHED_EXPIRATION_NOT_ADD,
                         reply,
                         value);
  }
  else
  {
    rc= text_incr_decr(instance, false, key, key_length, offset, reply, *value);
  }

  LIBMEMCACHED_MEMCACHED_DECREMENT_END();

  return rc;
}

memcached_return_t memcached_increment_with_initial(memcached_st *ptr,
                                                    const char *key,
                                                    size_t key_length,
                                                    uint64_t offset,
                                                    uint64_t initial,
                                                    time_t expiration,
                                                    uint64_t *value)
{
  return memcached_increment_with_initial_by_key(ptr, key, key_length,
                                                 key, key_length,
                                                 offset, initial, expiration, value);
}

memcached_return_t memcached_increment_with_initial_by_key(memcached_st *ptr,
                                                         const char *group_key,
                                                         size_t group_key_length,
                                                         const char *key,
                                                         size_t key_length,
                                                         uint64_t offset,
                                                         uint64_t initial,
                                                         time_t expiration,
                                                         uint64_t *value)
{
  uint64_t local_value;
  if (value == NULL)
  {
    value= &local_value;
  }

  memcached_return_t rc;
  if (memcached_failed(rc= initialize_query(ptr, true)))
  {
    return rc;
  }

  if (memcached_failed(rc= memcached_key_test(*ptr, (const char **)&key, &key_length, 1)))
  {
    return rc;
  }

  uint32_t server_key= memcached_generate_hash_with_redistribution(ptr, group_key, group_key_length);
  memcached_server_write_instance_st instance= memcached_server_instance_fetch(ptr, server_key);

  bool reply= memcached_is_replying(instance->root);

  LIBMEMCACHED_MEMCACHED_INCREMENT_WITH_INITIAL_START();
  if (memcached_is_binary(ptr))
  {
    rc= binary_incr_decr(instance, PROTOCOL_BINARY_CMD_INCREMENT,
                         key, key_length,
                         offset, initial, uint32_t(expiration),
                         reply,
                         value);
  }
  else
  {
    rc=  memcached_set_error(*ptr, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
                             memcached_literal_param("memcached_increment_with_initial_by_key() is not supported via the ASCII protocol"));
  }

  LIBMEMCACHED_MEMCACHED_INCREMENT_WITH_INITIAL_END();

  return rc;
}

memcached_return_t memcached_decrement_with_initial(memcached_st *ptr,
                                                    const char *key,
                                                    size_t key_length,
                                                    uint64_t offset,
                                                    uint64_t initial,
                                                    time_t expiration,
                                                    uint64_t *value)
{
  return memcached_decrement_with_initial_by_key(ptr, key, key_length,
                                                 key, key_length,
                                                 offset, initial, expiration, value);
}

memcached_return_t memcached_decrement_with_initial_by_key(memcached_st *ptr,
                                                           const char *group_key,
                                                           size_t group_key_length,
                                                           const char *key,
                                                           size_t key_length,
                                                           uint64_t offset,
                                                           uint64_t initial,
                                                           time_t expiration,
                                                           uint64_t *value)
{
  uint64_t local_value;
  if (value == NULL)
  {
    value= &local_value;
  }

  memcached_return_t rc;
  if (memcached_failed(rc= initialize_query(ptr, true)))
  {
    return rc;
  }

  if (memcached_failed(rc= memcached_key_test(*ptr, (const char **)&key, &key_length, 1)))
  {
    return rc;
  }

  uint32_t server_key= memcached_generate_hash_with_redistribution(ptr, group_key, group_key_length);
  memcached_server_write_instance_st instance= memcached_server_instance_fetch(ptr, server_key);

  bool reply= memcached_is_replying(instance->root);


  LIBMEMCACHED_MEMCACHED_INCREMENT_WITH_INITIAL_START();
  if (memcached_is_binary(ptr))
  {
    rc= binary_incr_decr(instance, PROTOCOL_BINARY_CMD_DECREMENT,
                         key, key_length,
                         offset, initial, uint32_t(expiration),
                         reply,
                         value);
  }
  else
  {
    rc=  memcached_set_error(*ptr, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
                             memcached_literal_param("memcached_decrement_with_initial_by_key() is not supported via the ASCII protocol"));
  }

  LIBMEMCACHED_MEMCACHED_INCREMENT_WITH_INITIAL_END();

  return rc;
}
