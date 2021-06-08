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
#include <cassert>

struct local_context {
  uint8_t major_version;
  uint8_t minor_version;
  uint8_t micro_version;

  bool truth;
};

static memcached_return_t
check_server_version(const memcached_st *, const memcached_instance_st *instance, void *context) {
  /* Do Nothing */
  struct local_context *check = (struct local_context *) context;

  if (memcached_server_major_version(instance) != UINT8_MAX) {
    uint32_t sv, cv;

    sv = memcached_server_micro_version(instance) | memcached_server_minor_version(instance) << 8
        | memcached_server_major_version(instance) << 16;
    cv = check->micro_version | check->minor_version << 8 | check->major_version << 16;

    if (sv >= cv) {
      return MEMCACHED_SUCCESS;
    }
  }

  check->truth = false;

  return MEMCACHED_FAILURE;
}

bool libmemcached_util_version_check(memcached_st *memc, uint8_t major_version,
                                     uint8_t minor_version, uint8_t micro_version) {
  if (memcached_failed(memcached_version(memc))) {
    return false;
  }

  struct local_context check = {major_version, minor_version, micro_version, true};

  memcached_server_fn callbacks[1];
  callbacks[0] = check_server_version;
  memcached_server_cursor(memc, callbacks, (void *) &check, 1);

  return check.truth;
}
