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

bool memcached_flag(const memcached_st &memc, const memcached_flag_t flag) {
  switch (flag) {
  case MEMCACHED_FLAG_AUTO_EJECT_HOSTS:
    return memcached_is_auto_eject_hosts(&memc);

  case MEMCACHED_FLAG_BINARY_PROTOCOL:
    return memcached_is_binary(&memc);

  case MEMCACHED_FLAG_BUFFER_REQUESTS:
    return memcached_is_buffering(&memc);

  case MEMCACHED_FLAG_HASH_WITH_NAMESPACE:
    return memcached_is_hash_with_namespace(&memc);

  case MEMCACHED_FLAG_NO_BLOCK:
    return memcached_is_no_block(&memc);

  case MEMCACHED_FLAG_REPLY:
    return memcached_is_replying(&memc);

  case MEMCACHED_FLAG_RANDOMIZE_REPLICA_READ:
    return memcached_is_randomize_replica_read(&memc);

  case MEMCACHED_FLAG_SUPPORT_CAS:
    return memcached_is_cas(&memc);

  case MEMCACHED_FLAG_TCP_NODELAY:
    return memcached_is_tcp_nodelay(&memc);

  case MEMCACHED_FLAG_USE_SORT_HOSTS:
    return memcached_is_use_sort_hosts(&memc);

  case MEMCACHED_FLAG_USE_UDP:
    return memcached_is_udp(&memc);

  case MEMCACHED_FLAG_VERIFY_KEY:
    return memcached_is_verify_key(&memc);

  case MEMCACHED_FLAG_TCP_KEEPALIVE:
    return memcached_is_use_sort_hosts(&memc);

  case MEMCACHED_FLAG_IS_AES:
    return memcached_is_aes(&memc);

  case MEMCACHED_FLAG_IS_FETCHING_VERSION:
    return memcached_is_fetching_version(&memc);
  }

  abort();
}

void memcached_flag(memcached_st &memc, const memcached_flag_t flag, const bool arg) {
  switch (flag) {
  case MEMCACHED_FLAG_AUTO_EJECT_HOSTS:
    memcached_set_auto_eject_hosts(memc, arg);
    break;

  case MEMCACHED_FLAG_BINARY_PROTOCOL:
    memcached_set_binary(memc, arg);
    break;

  case MEMCACHED_FLAG_BUFFER_REQUESTS:
    memcached_set_buffering(memc, arg);
    break;

  case MEMCACHED_FLAG_HASH_WITH_NAMESPACE:
    memcached_set_hash_with_namespace(memc, arg);
    break;

  case MEMCACHED_FLAG_NO_BLOCK:
    memcached_set_no_block(memc, arg);
    break;

  case MEMCACHED_FLAG_REPLY:
    memcached_set_replying(memc, arg);
    break;

  case MEMCACHED_FLAG_RANDOMIZE_REPLICA_READ:
    memcached_set_randomize_replica_read(memc, arg);
    break;

  case MEMCACHED_FLAG_SUPPORT_CAS:
    memcached_set_cas(memc, arg);
    break;

  case MEMCACHED_FLAG_TCP_NODELAY:
    memcached_set_tcp_nodelay(memc, arg);
    break;

  case MEMCACHED_FLAG_USE_SORT_HOSTS:
    memcached_set_use_sort_hosts(memc, arg);
    break;

  case MEMCACHED_FLAG_USE_UDP:
    memcached_set_udp(memc, arg);
    break;

  case MEMCACHED_FLAG_VERIFY_KEY:
    memcached_set_verify_key(memc, arg);
    break;

  case MEMCACHED_FLAG_TCP_KEEPALIVE:
    memcached_set_use_sort_hosts(memc, arg);
    break;

  case MEMCACHED_FLAG_IS_AES:
    memcached_set_aes(memc, arg);
    break;

  case MEMCACHED_FLAG_IS_FETCHING_VERSION:
    memcached_set_fetching_version(memc, arg);
    break;
  }
}
