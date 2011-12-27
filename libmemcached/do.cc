/* LibMemcached
 * Copyright (C) 2006-2010 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary:
 *
 */

#include <libmemcached/common.h>

memcached_return_t memcached_vdo(memcached_server_write_instance_st ptr,
                                 const struct libmemcached_io_vector_st *vector,
                                 const size_t count,
                                 const bool with_flush)
{
  memcached_return_t rc;

  WATCHPOINT_ASSERT(count);
  WATCHPOINT_ASSERT(vector);

  if (memcached_failed(rc= memcached_connect(ptr)))
  {
    WATCHPOINT_ERROR(rc);
    assert_msg(ptr->error_messages, "memcached_connect() returned an error but the memcached_server_write_instance_st showed none.");
    return rc;
  }

  /*
  ** Since non buffering ops in UDP mode dont check to make sure they will fit
  ** before they start writing, if there is any data in buffer, clear it out,
  ** otherwise we might get a partial write.
  **/
  if (memcached_is_udp(ptr->root) and with_flush and ptr->write_buffer_offset > UDP_DATAGRAM_HEADER_LENGTH)
  {
    if (memcached_io_write(ptr) == false)
    {
      memcached_io_reset(ptr);
      return memcached_set_error(*ptr, MEMCACHED_WRITE_FAILURE, MEMCACHED_AT);
    }
  }

  ssize_t sent_length= memcached_io_writev(ptr, vector, count, with_flush);

  size_t command_length= 0;
  for (uint32_t x= 0; x < count; ++x, vector++)
  {
    command_length+= vector->length;
  }

  if (sent_length == -1 or size_t(sent_length) != command_length)
  {
    rc= MEMCACHED_WRITE_FAILURE;
    WATCHPOINT_ERROR(rc);
    WATCHPOINT_ERRNO(errno);
  }
  else if ((ptr->root->flags.no_reply) == 0)
  {
    memcached_server_response_increment(ptr);
  }

  return rc;
}
