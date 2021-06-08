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

#ifndef MS_THREAD_H
#define MS_THREAD_H

#include <pthread.h>
#include <sched.h>
#include "ms_conn.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Time relative to server start. Smaller than time_t on 64-bit systems. */
typedef unsigned int rel_time_t;

/* Used to store the context of each thread */
typedef struct thread_ctx {
  uint32_t thd_idx;         /* the thread index */
  uint32_t nconns;          /* how many connections included by the thread */
  uint32_t srv_idx;         /* index of the thread */
  int tps_perconn;          /* expected throughput per connection */
  int64_t exec_num_perconn; /* execute number per connection */
  pthread_t pth_id;
} ms_thread_ctx_t;

/* Used to store the private variables of each thread */
typedef struct thread {
  ms_conn_t *conn;       /* conn array to store all the conn in the thread */
  uint32_t nactive_conn; /* how many connects are active */

  ms_thread_ctx_t *thread_ctx; /* thread context from the caller */
  struct event_base *base;     /* libevent handler created by this thread */

  rel_time_t curr_time;     /* current time */
  struct event clock_event; /* clock event to time each one second */
  bool initialized;         /* whether clock_event has been initialized */

  struct timeval startup_time; /* start time of the thread */
} ms_thread_t;

/* initialize threads */
void ms_thread_init(void);

/* cleanup some resource of threads when all the threads exit */
void ms_thread_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* end of MS_THREAD_H */
