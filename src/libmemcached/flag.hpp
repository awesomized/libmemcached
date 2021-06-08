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

#pragma once

enum memcached_flag_t {
  MEMCACHED_FLAG_AUTO_EJECT_HOSTS,
  MEMCACHED_FLAG_BINARY_PROTOCOL,
  MEMCACHED_FLAG_BUFFER_REQUESTS,
  MEMCACHED_FLAG_HASH_WITH_NAMESPACE,
  MEMCACHED_FLAG_NO_BLOCK,
  MEMCACHED_FLAG_REPLY,
  MEMCACHED_FLAG_RANDOMIZE_REPLICA_READ,
  MEMCACHED_FLAG_SUPPORT_CAS,
  MEMCACHED_FLAG_TCP_NODELAY,
  MEMCACHED_FLAG_USE_SORT_HOSTS,
  MEMCACHED_FLAG_USE_UDP,
  MEMCACHED_FLAG_VERIFY_KEY,
  MEMCACHED_FLAG_TCP_KEEPALIVE,
  MEMCACHED_FLAG_IS_AES,
  MEMCACHED_FLAG_IS_FETCHING_VERSION
};

bool memcached_flag(const memcached_st &, const memcached_flag_t);
void memcached_flag(memcached_st &, const memcached_flag_t, const bool);
