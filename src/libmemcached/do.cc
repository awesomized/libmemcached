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

static memcached_return_t _vdo_udp(memcached_instance_st *instance,
                                   libmemcached_io_vector_st vector[], const size_t count) {
#if HAVE_SENDMSG && HAVE_STRUCT_MSGHDR
  if (vector[0].buffer or vector[0].length) {
    return memcached_set_error(
        *instance->root, MEMCACHED_NOT_SUPPORTED, MEMCACHED_AT,
        memcached_literal_param("UDP messages was attempted, but vector was not setup for it"));
  }

  struct msghdr msg;
  memset(&msg, 0, sizeof(msg));

  increment_udp_message_id(instance);
  vector[0].buffer = instance->write_buffer;
  vector[0].length = UDP_DATAGRAM_HEADER_LENGTH;

  msg.msg_iov = (struct iovec *) vector;
#  ifdef __APPLE__
  msg.msg_iovlen = int(count);
#  else
  msg.msg_iovlen = count;
#  endif

  uint32_t retry = 5;
  while (--retry) {
    ssize_t sendmsg_length = ::sendmsg(instance->fd, &msg, 0);
    if (sendmsg_length > 0) {
      break;
    } else if (sendmsg_length < 0) {
      if (errno == EMSGSIZE) {
        return memcached_set_error(*instance, MEMCACHED_WRITE_FAILURE, MEMCACHED_AT);
      }

      return memcached_set_errno(*instance, errno, MEMCACHED_AT);
    }
  }

  return MEMCACHED_SUCCESS;
#else
  (void) instance;
  (void) vector;
  (void) count;
  return MEMCACHED_FAILURE;
#endif
}

memcached_return_t memcached_vdo(memcached_instance_st *instance,
                                 libmemcached_io_vector_st vector[], const size_t count,
                                 const bool with_flush) {
  memcached_return_t rc;

  assert_msg(vector, "Invalid vector passed");

  if (memcached_failed(rc = memcached_connect(instance))) {
    WATCHPOINT_ERROR(rc);
    assert_msg(instance->error_messages,
               "memcached_connect() returned an error but the Instance showed none.");
    return rc;
  }

  /*
  ** Since non buffering ops in UDP mode dont check to make sure they will fit
  ** before they start writing, if there is any data in buffer, clear it out,
  ** otherwise we might get a partial write.
  **/
  bool sent_success;
  if (memcached_is_udp(instance->root)) {
    sent_success = memcached_success(rc = _vdo_udp(instance, vector, count));
  } else {
    sent_success = memcached_io_writev(instance, vector, count, with_flush);
  }
  if (sent_success == false) {
    rc = memcached_last_error(instance->root);
    if (rc == MEMCACHED_SUCCESS) {
      memcached_set_error(*instance, MEMCACHED_WRITE_FAILURE, MEMCACHED_AT);
    }
    memcached_io_reset(instance);
  } else if (memcached_is_replying(instance->root) && !memcached_is_udp(instance->root)) {
    memcached_server_response_increment(instance);
  }

  return rc;
}
