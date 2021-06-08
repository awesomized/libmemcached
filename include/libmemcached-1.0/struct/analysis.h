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

struct memcached_analysis_st {
  memcached_st *root;
  uint32_t average_item_size;
  uint32_t longest_uptime;
  uint32_t least_free_server;
  uint32_t most_consumed_server;
  uint32_t oldest_server;
  double pool_hit_ratio;
  uint64_t most_used_bytes;
  uint64_t least_remaining_bytes;
};
