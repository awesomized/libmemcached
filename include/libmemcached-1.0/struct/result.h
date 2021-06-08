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

struct memcached_result_st {
  uint32_t item_flags;
  time_t item_expiration;
  size_t key_length;
  uint64_t item_cas;
  struct memcached_st *root;
  memcached_string_st value;
  uint64_t numeric_value;
  uint64_t count;
  char item_key[MEMCACHED_MAX_KEY];
  struct {
    bool is_allocated : 1;
    bool is_initialized : 1;
  } options;
  /* Add result callback function */
};
