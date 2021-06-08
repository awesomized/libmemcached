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
#include "libmemcached/csl/parser.h"

class Context {
public:
  Context(const char *option_string, size_t option_string_length, memcached_st *memc_,
          memcached_return_t &rc_arg)
  : previous_token(END)
  , scanner(NULL)
  , buf(option_string)
  , begin(NULL)
  , pos(0)
  , length(option_string_length)
  , memc(memc_)
  , rc(rc_arg)
  , _is_server(false)
  , _end(false)
  , _has_hash(false) {
    _hostname[0] = 0;
    init_scanner();
    rc = MEMCACHED_SUCCESS;

    memc->state.is_parsing = true;
    memcached_string_create(memc, &_string_buffer, 1024);
  }

  bool end() {
    return _end;
  }

  void start();

  void set_end() {
    rc = MEMCACHED_SUCCESS;
    _end = true;
  }

  bool set_hash(memcached_hash_t hash);

  void set_server() {
    _is_server = true;
  }

  void unset_server() {
    _is_server = false;
  }

  bool is_server() const {
    return _is_server;
  }

  void hostname(const char *, size_t, server_t &);

  bool string_buffer(const char *, size_t, memcached_string_t &);

  const char *hostname() const {
    return _hostname;
  }

  void abort(const char *, config_tokentype, const char *);
  void error(const char *, config_tokentype, const char *);

  ~Context() {
    memcached_string_free(&_string_buffer);
    destroy_scanner();
    memc->state.is_parsing = false;
  }

  config_tokentype previous_token;
  void *scanner;
  const char *buf;
  const char *begin;
  size_t pos;
  size_t length;
  memcached_st *memc;
  memcached_return_t &rc;

protected:
  void init_scanner();
  void destroy_scanner();

private:
  bool _is_server;
  bool _end;
  char _hostname[NI_MAXHOST];
  bool _has_hash;
  memcached_string_st _string_buffer;
};
