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

bool libmemcached_util_ping(const char *hostname, in_port_t port, memcached_return_t *ret) {
  memcached_return_t unused;
  if (ret == NULL) {
    ret = &unused;
  }

  memcached_st *memc_ptr = memcached_create(NULL);
  if (memc_ptr == NULL) {
    *ret = MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    return false;
  }

  if (memcached_success(
          (*ret = memcached_behavior_set(memc_ptr, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 400000))))
  {
    memcached_return_t rc = memcached_server_add(memc_ptr, hostname, port);
    if (memcached_success(rc)) {
      rc = memcached_version(memc_ptr);
    }

    if (memcached_failed(rc) and rc == MEMCACHED_SOME_ERRORS) {
      const memcached_instance_st *instance = memcached_server_instance_by_position(memc_ptr, 0);

      assert_msg(instance and memcached_server_error(instance), " ");
      if (instance and memcached_server_error(instance)) {
        rc = memcached_server_error_return(instance);
      }
    }

    *ret = rc;
  }
  memcached_free(memc_ptr);

  return memcached_success(*ret);
}

bool libmemcached_util_ping2(const char *hostname, in_port_t port, const char *username,
                             const char *password, memcached_return_t *ret) {
  if (username == NULL) {
    return libmemcached_util_ping(hostname, port, ret);
  }

  memcached_return_t unused;
  if (not ret)
    ret = &unused;

  if (LIBMEMCACHED_WITH_SASL_SUPPORT == 0) {
    *ret = MEMCACHED_NOT_SUPPORTED;
    return false;
  }

  memcached_st *memc_ptr = memcached_create(NULL);
  if (not memc_ptr) {
    *ret = MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    return false;
  }

  if (memcached_failed(*ret = memcached_set_sasl_auth_data(memc_ptr, username, password))) {
    memcached_free(memc_ptr);
    return false;
  }

  memcached_return_t rc = memcached_server_add(memc_ptr, hostname, port);
  if (memcached_success(rc)) {
    rc = memcached_version(memc_ptr);
  }

  if (memcached_failed(rc) and rc == MEMCACHED_SOME_ERRORS) {
    const memcached_instance_st *instance = memcached_server_instance_by_position(memc_ptr, 0);

    assert_msg(instance and memcached_server_error(instance), " ");
    if (instance and memcached_server_error(instance)) {
      rc = memcached_server_error_return(instance);
    }
  }
  memcached_free(memc_ptr);

  *ret = rc;

  return memcached_success(rc);
}
