#include "common.h"

static memcached_return_t memcached_flush_binary(memcached_st *ptr, 
                                                 time_t expiration);
static memcached_return_t memcached_flush_textual(memcached_st *ptr, 
                                                  time_t expiration);

memcached_return_t memcached_flush(memcached_st *ptr, time_t expiration)
{
  memcached_return_t rc;
  if ((rc= initialize_query(ptr)) != MEMCACHED_SUCCESS)
  {
    return rc;
  }

  LIBMEMCACHED_MEMCACHED_FLUSH_START();
  if (ptr->flags.binary_protocol)
    rc= memcached_flush_binary(ptr, expiration);
  else
    rc= memcached_flush_textual(ptr, expiration);
  LIBMEMCACHED_MEMCACHED_FLUSH_END();
  return rc;
}

static memcached_return_t memcached_flush_textual(memcached_st *ptr, 
                                                  time_t expiration)
{
  unlikely (memcached_server_count(ptr) == 0)
    return MEMCACHED_NO_SERVERS;

  for (unsigned int x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_return_t rc;
    char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

    bool no_reply= ptr->flags.no_reply;
    memcached_server_write_instance_st instance=
      memcached_server_instance_fetch(ptr, x);

    int send_length;
    if (expiration)
    {
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all %llu%s\r\n",
                            (unsigned long long)expiration, no_reply ? " noreply" : "");
    }
    else
    {
      send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                            "flush_all%s\r\n", no_reply ? " noreply" : "");
    }

    if (send_length >= MEMCACHED_DEFAULT_COMMAND_SIZE || send_length < 0)
    {
      return MEMCACHED_FAILURE;
    }

    rc= memcached_do(instance, buffer, (size_t)send_length, true);

    if (rc == MEMCACHED_SUCCESS && !no_reply)
      (void)memcached_response(instance, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL);
  }

  return MEMCACHED_SUCCESS;
}

static memcached_return_t memcached_flush_binary(memcached_st *ptr, 
                                                 time_t expiration)
{
  protocol_binary_request_flush request= {.bytes= {0}};

  unlikely (memcached_server_count(ptr) == 0)
    return MEMCACHED_NO_SERVERS;

  request.message.header.request.magic= (uint8_t)PROTOCOL_BINARY_REQ;
  request.message.header.request.opcode= PROTOCOL_BINARY_CMD_FLUSH;
  request.message.header.request.extlen= 4;
  request.message.header.request.datatype= PROTOCOL_BINARY_RAW_BYTES;
  request.message.header.request.bodylen= htonl(request.message.header.request.extlen);
  request.message.body.expiration= htonl((uint32_t) expiration);

  for (uint32_t x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_server_write_instance_st instance=
      memcached_server_instance_fetch(ptr, x);

    if (ptr->flags.no_reply)
    {
      request.message.header.request.opcode= PROTOCOL_BINARY_CMD_FLUSHQ;
    }
    else
    {
      request.message.header.request.opcode= PROTOCOL_BINARY_CMD_FLUSH;
    }

    if (memcached_do(instance, request.bytes, sizeof(request.bytes), true) != MEMCACHED_SUCCESS) 
    {
      memcached_io_reset(instance);
      return MEMCACHED_WRITE_FAILURE;
    } 
  }

  for (uint32_t x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_server_write_instance_st instance=
      memcached_server_instance_fetch(ptr, x);

    if (memcached_server_response_count(instance) > 0)
      (void)memcached_response(instance, NULL, 0, NULL);
  }

  return MEMCACHED_SUCCESS;
}
