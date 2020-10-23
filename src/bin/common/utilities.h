/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
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

#pragma once

#include <getopt.h>
#include "libmemcached-1.0/memcached.h"
#include "client_options.h"

#if defined(HAVE_SYS_TIME_H)
#  include <sys/time.h>
#endif

#if defined(HAVE_TIME_H)
#  include <time.h>
#endif

#ifdef __sun
/* For some odd reason the option struct on solaris defines the argument
 * as char* and not const char*
 */
#  define OPTIONSTRING char *
#else
#  define OPTIONSTRING const char *
#endif

typedef struct memcached_programs_help_st memcached_programs_help_st;

struct memcached_programs_help_st {
  char *not_used_yet;
};

#ifdef __cplusplus
extern "C" {
#endif

char *strdup_cleanup(const char *str);
void cleanup(void);
long int timedif(struct timeval a, struct timeval b);
void version_command(const char *command_name) __attribute__((noreturn));
void help_command(const char *command_name, const char *description,
                  const struct option *long_options, memcached_programs_help_st *options)
    __attribute__((noreturn));
void process_hash_option(memcached_st *memc, char *opt_hash);
bool initialize_sasl(memcached_st *memc, char *user, char *password);
void shutdown_sasl(void);
void initialize_sockets(void);
void close_stdio(void);

#ifdef __cplusplus
} // extern "C"
#endif
