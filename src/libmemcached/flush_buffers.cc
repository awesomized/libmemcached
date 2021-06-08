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

memcached_return_t memcached_flush_buffers(memcached_st *shell) {
  Memcached *memc = memcached2Memcached(shell);
  if (memc) {
    memcached_return_t ret = MEMCACHED_SUCCESS;

    for (uint32_t x = 0; x < memcached_server_count(memc); ++x) {
      memcached_instance_st *instance = memcached_instance_fetch(memc, x);

      if (instance->write_buffer_offset) {
        if (instance->fd == INVALID_SOCKET
            and (ret = memcached_connect(instance)) != MEMCACHED_SUCCESS) {
          WATCHPOINT_ERROR(ret);
          return ret;
        }

        if (memcached_io_write(instance) == false) {
          ret = MEMCACHED_SOME_ERRORS;
        }
      }
    }

    return ret;
  }

  return MEMCACHED_INVALID_ARGUMENTS;
}
