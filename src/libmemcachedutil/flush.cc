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

#include "libmemcachedutil/common.h"

bool libmemcached_util_flush(const char *hostname, in_port_t port, memcached_return_t *ret) {
  memcached_st *memc_ptr = memcached_create(NULL);

  memcached_return_t rc = memcached_server_add(memc_ptr, hostname, port);
  if (memcached_success(rc)) {
    rc = memcached_flush(memc_ptr, 0);
  }

  memcached_free(memc_ptr);

  if (ret) {
    *ret = rc;
  }

  return memcached_success(rc);
}
