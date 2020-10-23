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

typedef struct memcached_help_text_st memcached_help_text_st;

enum memcached_options {
  OPT_SERVERS = 's',
  OPT_VERSION = 'V',
  OPT_HELP = 'h',
  OPT_VERBOSE = 'v',
  OPT_DEBUG = 'd',
  OPT_ANALYZE = 'a',
  OPT_FLAG = 257,
  OPT_EXPIRE,
  OPT_SET,
  OPT_REPLACE,
  OPT_ADD,
  OPT_SLAP_EXECUTE_NUMBER,
  OPT_SLAP_INITIAL_LOAD,
  OPT_SLAP_TEST,
  OPT_SLAP_CONCURRENCY,
  OPT_SLAP_NON_BLOCK,
  OPT_SLAP_TCP_NODELAY,
  OPT_FLUSH,
  OPT_HASH,
  OPT_BINARY,
  OPT_UDP,
  OPT_BUFFER,
  OPT_USERNAME,
  OPT_PASSWD,
  OPT_STAT_ARGS,
  OPT_SERVER_VERSION,
  OPT_QUIET,
  OPT_FILE = 'f'
};
