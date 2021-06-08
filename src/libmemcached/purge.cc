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

#define memcached_set_purging(object_, value_) ((object_)->state.is_purging = (value_))

class Purge {
public:
  Purge(Memcached *arg)
  : _memc(arg) {
    memcached_set_purging(_memc, true);
  }

  ~Purge() {
    memcached_set_purging(_memc, false);
  }

private:
  Memcached *_memc;
};

class PollTimeout {
public:
  PollTimeout(Memcached *arg, int32_t ms = 50)
  : _timeout(arg->poll_timeout)
  , _origin(arg->poll_timeout) {
    _origin = ms;
  }

  ~PollTimeout() {
    _origin = _timeout;
  }

private:
  int32_t _timeout;
  int32_t &_origin;
};

bool memcached_purge(memcached_instance_st *ptr) {
  Memcached *root = (Memcached *) ptr->root;

  if (memcached_is_purging(ptr->root) || /* already purging */
      (memcached_server_response_count(ptr) < ptr->root->io_msg_watermark
       && ptr->io_bytes_sent < ptr->root->io_bytes_watermark)
      || (ptr->io_bytes_sent >= ptr->root->io_bytes_watermark
          && memcached_server_response_count(ptr) < 2))
  {
    return true;
  }

  /*
    memcached_io_write and memcached_response may call memcached_purge
    so we need to be able stop any recursion..
  */
  Purge set_purge(root);

  WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
  /*
    Force a flush of the buffer to ensure that we don't have the n-1 pending
    requests buffered up..
  */
  if (memcached_io_write(ptr) == false) {
    memcached_io_reset(ptr);
    memcached_set_error(*ptr, MEMCACHED_WRITE_FAILURE, MEMCACHED_AT);
    return false;
  }
  WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);

  bool is_successful = true;
  uint32_t no_msg = memcached_server_response_count(ptr);
  if (no_msg > 1) {
    memcached_result_st result;

    /*
     * We need to increase the timeout, because we might be waiting for
     * data to be sent from the server (the commands was in the output buffer
     * and just flushed
     */
    PollTimeout poll_timeout(ptr->root);

    memcached_result_st *result_ptr = memcached_result_create(root, &result);
    assert(result_ptr);

    for (uint32_t x = 0; x < no_msg - 1; x++) {
      memcached_result_reset(result_ptr);
      memcached_return_t rc = memcached_read_one_response(ptr, result_ptr);
      /*
       * Purge doesn't care for what kind of command results that is received.
       * The only kind of errors I care about if is I'm out of sync with the
       * protocol or have problems reading data from the network..
       */
      if (rc == MEMCACHED_PROTOCOL_ERROR or rc == MEMCACHED_UNKNOWN_READ_FAILURE
          or rc == MEMCACHED_READ_FAILURE)
      {
        WATCHPOINT_ERROR(rc);
        is_successful = false;
      }
      if (rc == MEMCACHED_TIMEOUT) {
        break;
      }

      if (ptr->root->callbacks) {
        memcached_callback_st cb = *ptr->root->callbacks;
        if (memcached_success(rc)) {
          for (uint32_t y = 0; y < cb.number_of_callback; y++) {
            if (memcached_fatal((*cb.callback[y])(ptr->root, result_ptr, cb.context))) {
              break;
            }
          }
        }
      }
    }

    memcached_result_free(result_ptr);
  }

  return is_successful;
}
