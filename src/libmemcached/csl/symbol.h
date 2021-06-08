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

#include "libmemcached/csl/common.h"

union CONFIG_STYPE {
  long long number;
  memcached_string_t string;
  memcached_string_t option;
  double double_number;
  memcached_server_distribution_t distribution;
  memcached_hash_t hash;
  memcached_behavior_t behavior;
  bool boolean;
  server_t server;
};

typedef union CONFIG_STYPE CONFIG_STYPE;
