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

// Never look at the stat object directly.

pid_t libmemcached_util_getpid(const char *hostname, in_port_t port, memcached_return_t *ret) {
  pid_t pid = -1;

  memcached_return_t unused;
  if (ret == NULL) {
    ret = &unused;
  }

  memcached_st *memc_ptr = memcached_create(NULL);
  if (memc_ptr == NULL) {
    *ret = MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    return -1;
  }

  memcached_return_t rc = memcached_server_add(memc_ptr, hostname, port);
  if (memcached_success(rc)) {
    memcached_stat_st *stat = memcached_stat(memc_ptr, NULL, &rc);
    if (memcached_success(rc) and stat and stat->pid != -1) {
      pid = stat->pid;
    } else if (memcached_success(rc)) {
      rc = MEMCACHED_UNKNOWN_STAT_KEY;      // Something went wrong if this happens
    } else if (rc == MEMCACHED_SOME_ERRORS) // Generic answer, we will now find the specific reason
                                            // (if one exists)
    {
      const memcached_instance_st *instance = memcached_server_instance_by_position(memc_ptr, 0);

      assert_msg(instance and memcached_server_error(instance), " ");
      if (instance and memcached_server_error(instance)) {
        rc = memcached_server_error_return(instance);
      }
    }

    memcached_stat_free(memc_ptr, stat);
  }
  memcached_free(memc_ptr);

  *ret = rc;

  return pid;
}

pid_t libmemcached_util_getpid2(const char *hostname, in_port_t port, const char *username,
                                const char *password, memcached_return_t *ret) {
  if (username == NULL) {
    return libmemcached_util_getpid(hostname, port, ret);
  }

  pid_t pid = -1;

  memcached_return_t unused;
  if (not ret)
    ret = &unused;

  if (LIBMEMCACHED_WITH_SASL_SUPPORT == 0) {
    *ret = MEMCACHED_NOT_SUPPORTED;
    return pid;
  }

  memcached_st *memc_ptr = memcached_create(NULL);
  if (not memc_ptr) {
    *ret = MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    return -1;
  }

  if (memcached_failed(*ret = memcached_set_sasl_auth_data(memc_ptr, username, password))) {
    memcached_free(memc_ptr);
    return false;
  }

  memcached_return_t rc = memcached_server_add(memc_ptr, hostname, port);
  if (memcached_success(rc)) {
    memcached_stat_st *stat = memcached_stat(memc_ptr, NULL, &rc);
    if (memcached_success(rc) and stat and stat->pid != -1) {
      pid = stat->pid;
    } else if (memcached_success(rc)) {
      rc = MEMCACHED_UNKNOWN_STAT_KEY;      // Something went wrong if this happens
    } else if (rc == MEMCACHED_SOME_ERRORS) // Generic answer, we will now find the specific reason
                                            // (if one exists)
    {
      const memcached_instance_st *instance = memcached_server_instance_by_position(memc_ptr, 0);

#if 0
      assert_msg(instance and instance->error_messages, " ");
#endif
      if (instance and memcached_server_error(instance)) {
        rc = memcached_server_error_return(instance);
      }
    }

    memcached_stat_free(memc_ptr, stat);
  }
  memcached_free(memc_ptr);

  *ret = rc;

  return pid;
}
