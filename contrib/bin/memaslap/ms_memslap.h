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

#ifndef MS_MEMSLAP_H
#define MS_MEMSLAP_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#if !defined(__cplusplus)
#  include <stdbool.h>
#endif
#include <math.h>

#include "ms_stats.h"
#include "ms_atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

/* command line option  */
typedef enum {
  OPT_VERSION = 'V',
  OPT_HELP = 'h',
  OPT_UDP = 'U',
  OPT_SERVERS = 's',
  OPT_EXECUTE_NUMBER = 'x',
  OPT_THREAD_NUMBER = 'T',
  OPT_CONCURRENCY = 'c',
  OPT_FIXED_LTH = 'X',
  OPT_VERIFY = 'v',
  OPT_GETS_DIVISION = 'd',
  OPT_TIME = 't',
  OPT_CONFIG_CMD = 'F',
  OPT_WINDOW_SIZE = 'w',
  OPT_EXPIRE = 'e',
  OPT_STAT_FREQ = 'S',
  OPT_RECONNECT = 'R',
  OPT_VERBOSE = 'b',
  OPT_FACEBOOK_TEST = 'a',
  OPT_SOCK_PER_CONN = 'n',
  OPT_BINARY_PROTOCOL = 'B',
  OPT_OVERWRITE = 'o',
  OPT_TPS = 'P',
  OPT_REP_WRITE_SRV = 'p'
} ms_options_t;

/* global statistic of response time */
typedef struct statistic {
  pthread_mutex_t stat_mutex; /* synchronize the following members */

  ms_stat_t get_stat;   /* statistics of get command */
  ms_stat_t set_stat;   /* statistics of set command */
  ms_stat_t total_stat; /* statistics of both get and set commands */
} ms_statistic_t;

/* global status statistic structure */
typedef struct stats {
  ATOMIC uint32_t active_conns; /* active connections */
  ATOMIC size_t bytes_read;     /* read bytes */
  ATOMIC size_t bytes_written;  /* written bytes */
  ATOMIC size_t obj_bytes;      /* object bytes */
  ATOMIC size_t pre_cmd_get;    /* previous total get command count */
  ATOMIC size_t pre_cmd_set;    /* previous total set command count */
  ATOMIC size_t cmd_get;        /* current total get command count */
  ATOMIC size_t cmd_set;        /* current total set command count */
  ATOMIC size_t get_misses;     /* total objects of get miss */
  ATOMIC size_t vef_miss;       /* total objects of verification miss  */
  ATOMIC size_t vef_failed;     /* total objects of verification failed  */
  ATOMIC size_t unexp_unget;    /* total objects which is unexpired but not get */
  ATOMIC size_t exp_get;        /* total objects which is expired but get  */
  ATOMIC size_t pkt_disorder;   /* disorder packages of UDP */
  ATOMIC size_t pkt_drop;       /* packages dropped of UDP */
  ATOMIC size_t udp_timeout;    /* how many times timeout of UDP happens */
} ms_stats_t;

/* lock adapter */
typedef struct sync_lock {
  uint32_t count;
  pthread_mutex_t lock;
  pthread_cond_t cond;
} ms_sync_lock_t;

/* global variable structure */
typedef struct global {
  /* synchronize lock */
  ms_sync_lock_t init_lock;
  ms_sync_lock_t warmup_lock;
  ms_sync_lock_t run_lock;

  /* mutex for outputing error log synchronously when memslap crashes */
  pthread_mutex_t quit_mutex;

  /* mutex for generating key prefix */
  pthread_mutex_t seq_mutex;

  /* global synchronous flags for slap mode */
  ATOMIC bool finish_warmup;
  ATOMIC bool time_out;
} ms_global_t;

/* global structure */
extern ms_global_t ms_global;

/* global stats information structure */
extern ms_stats_t ms_stats;

/* global statistic structure */
extern ms_statistic_t ms_statistic;

#ifdef __cplusplus
}
#endif

#endif /* end of MS_MEMSLAP_H */
