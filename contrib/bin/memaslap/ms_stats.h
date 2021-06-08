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
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#ifndef MS_STAT_H
#define MS_STAT_H

#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* statistic structure of response time */
typedef struct {
  char *name;
  uint64_t total_time;
  uint64_t min_time;
  uint64_t max_time;
  uint64_t get_miss;
  uint64_t dist[65];
  double squares;
  double log_product;

  uint64_t period_min_time;
  uint64_t period_max_time;
  uint64_t pre_get_miss;
  uint64_t pre_events;
  uint64_t pre_total_time;
  uint64_t pre_squares;
  double pre_log_product;
} ms_stat_t;

/* initialize statistic */
void ms_init_stats(ms_stat_t *stat, const char *name);

/* record one event */
void ms_record_event(ms_stat_t *stat, uint64_t time, int get_miss);

/* dump the statistics */
void ms_dump_stats(ms_stat_t *stat);

/* dump the format statistics */
void ms_dump_format_stats(ms_stat_t *stat, int run_time, int freq, int obj_size);

#ifdef __cplusplus
}
#endif

#endif /* MS_STAT_H */
