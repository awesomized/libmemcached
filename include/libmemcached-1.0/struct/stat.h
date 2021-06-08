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

struct memcached_stat_st {
  unsigned long connection_structures;
  unsigned long curr_connections;
  unsigned long curr_items;
  pid_t pid;
  unsigned long pointer_size;
  unsigned long rusage_system_microseconds;
  unsigned long rusage_system_seconds;
  unsigned long rusage_user_microseconds;
  unsigned long rusage_user_seconds;
  unsigned long threads;
  unsigned long time;
  unsigned long total_connections;
  unsigned long total_items;
  unsigned long uptime;
  unsigned long long bytes;
  unsigned long long bytes_read;
  unsigned long long bytes_written;
  unsigned long long cmd_get;
  unsigned long long cmd_set;
  unsigned long long evictions;
  unsigned long long get_hits;
  unsigned long long get_misses;
  unsigned long long limit_maxbytes;
  char version[MEMCACHED_VERSION_STRING_LENGTH];
  void *__future; // @todo create a new structure to place here for future usage
  memcached_st *root;
};
