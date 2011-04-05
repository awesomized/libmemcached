#include "common.h"

/*
  This closes all connections (forces flush of input as well).

  Maybe add a host specific, or key specific version?

  The reason we send "quit" is that in case we have buffered IO, this
  will force data to be completed.
*/

void memcached_quit_server(memcached_server_st *ptr, bool io_death)
{
  if (ptr->fd != INVALID_SOCKET)
  {
    if (io_death == false && ptr->type != MEMCACHED_CONNECTION_UDP && ptr->options.is_shutting_down == false)
    {
      memcached_return_t rc;
      char buffer[MEMCACHED_MAX_BUFFER];

      ptr->options.is_shutting_down= true;

      if (ptr->root->flags.binary_protocol)
      {
        protocol_binary_request_quit request = {.bytes= {0}};
        request.message.header.request.magic = PROTOCOL_BINARY_REQ;
        request.message.header.request.opcode = PROTOCOL_BINARY_CMD_QUIT;
        request.message.header.request.datatype = PROTOCOL_BINARY_RAW_BYTES;
        rc= memcached_do(ptr, request.bytes, sizeof(request.bytes), true);
      }
      else
      {
        rc= memcached_do(ptr, "quit\r\n", sizeof("quit\r\n") -1, true);
      }

      WATCHPOINT_ASSERT(rc == MEMCACHED_SUCCESS || rc == MEMCACHED_FETCH_NOTFINISHED);
      (void)rc; // Shut up ICC

      /* read until socket is closed, or there is an error
       * closing the socket before all data is read
       * results in server throwing away all data which is
       * not read
       *
       * In .40 we began to only do this if we had been doing buffered
       * requests of had replication enabled.
       */
      if (ptr->root->flags.buffer_requests || ptr->root->number_of_replicas)
      {
        ssize_t nread;
        while (memcached_io_read(ptr, buffer, sizeof(buffer)/sizeof(*buffer),
                                 &nread) == MEMCACHED_SUCCESS);
      }


      /*
       * memcached_io_read may call memcached_quit_server with io_death if
       * it encounters problems, but we don't care about those occurences.
       * The intention of that loop is to drain the data sent from the
       * server to ensure that the server processed all of the data we
       * sent to the server.
       */
      ptr->server_failure_counter= 0;
    }
    memcached_io_close(ptr);
  }

  ptr->fd= INVALID_SOCKET;
  ptr->io_bytes_sent= 0;
  ptr->write_buffer_offset= (size_t) ((ptr->type == MEMCACHED_CONNECTION_UDP) ? UDP_DATAGRAM_HEADER_LENGTH : 0);
  ptr->read_buffer_length= 0;
  ptr->read_ptr= ptr->read_buffer;
  ptr->options.is_shutting_down= false;
  memcached_server_response_reset(ptr);

  // We reset the version so that if we end up talking to a different server
  // we don't have stale server version information.
  ptr->major_version= ptr->minor_version= ptr->micro_version= UINT8_MAX;

  if (io_death)
  {
    ptr->server_failure_counter++;
    set_last_disconnected_host(ptr);
  }
}

void send_quit(memcached_st *ptr)
{
  for (uint32_t x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_server_write_instance_st instance=
      memcached_server_instance_fetch(ptr, x);

    memcached_quit_server(instance, false);
  }
}

void memcached_quit(memcached_st *ptr)
{
  if (initialize_query(ptr) != MEMCACHED_SUCCESS)
  {
    return;
  }

  send_quit(ptr);
}
