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

enum memcached_server_distribution_t {
  MEMCACHED_DISTRIBUTION_MODULA,
  MEMCACHED_DISTRIBUTION_CONSISTENT,
  MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA,
  MEMCACHED_DISTRIBUTION_RANDOM,
  MEMCACHED_DISTRIBUTION_CONSISTENT_KETAMA_SPY,
  MEMCACHED_DISTRIBUTION_CONSISTENT_WEIGHTED,
  MEMCACHED_DISTRIBUTION_VIRTUAL_BUCKET,
  MEMCACHED_DISTRIBUTION_CONSISTENT_MAX
};

#ifndef __cplusplus
typedef enum memcached_server_distribution_t memcached_server_distribution_t;
#endif
