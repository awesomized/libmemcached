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

#include "libmemcached/csl/common.h"
#include "libmemcached/csl/context.h"

void Context::abort(const char *error_arg, config_tokentype last_token,
                    const char *last_token_str) {
  rc = MEMCACHED_PARSE_ERROR;
  (void) last_token;

  if (error_arg) {
    memcached_set_parser_error(*memc, MEMCACHED_AT, "%s", error_arg);
    return;
  }

  if (last_token_str) {
    memcached_set_parser_error(*memc, MEMCACHED_AT, "%s", last_token_str);
    return;
  }

  memcached_set_parser_error(*memc, MEMCACHED_AT, "unknown parsing error");
}

void Context::error(const char *error_arg, config_tokentype last_token,
                    const char *last_token_str) {
  rc = MEMCACHED_PARSE_ERROR;
  if (not error_arg) {
    memcached_set_parser_error(*memc, MEMCACHED_AT, "Unknown error occured during parsing (%s)",
                               last_token_str ? last_token_str : " ");
    return;
  }

  if (strcmp(error_arg, "memory exhausted") == 0) {
    (void) memcached_set_error(*memc, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT,
                               memcached_string_make_from_cstr(error_arg));
    return;
  }

  // We now test if it is something other then a syntax error, if it  we
  // return a generic message
  if (strcmp(error_arg, "syntax error")) {
    memcached_set_parser_error(*memc, MEMCACHED_AT,
                               "Error occured during parsing (%s): last_token=%s(%d)", error_arg,
                               last_token_str, last_token);
    return;
  }

  if (last_token == UNKNOWN_OPTION and begin) {
    memcached_set_parser_error(*memc, MEMCACHED_AT, "Unknown option: %s", begin);
  } else if (last_token == UNKNOWN) {
    memcached_set_parser_error(*memc, MEMCACHED_AT,
                               "Error occured durring parsing, an unknown token was found.");
  } else {
    memcached_set_parser_error(*memc, MEMCACHED_AT, "Error occured while parsing (%s)",
                               last_token_str ? last_token_str : " ");
  }
}

void Context::hostname(const char *str, size_t size, server_t &server_) {
  size_t copy_length = size_t(NI_MAXHOST) > size ? size : size_t(NI_MAXHOST);
  memcpy(_hostname, str, copy_length);
  _hostname[copy_length] = 0;

  server_.port = MEMCACHED_DEFAULT_PORT;
  server_.weight = 1;
  server_.c_str = _hostname;
  server_.size = size;
}

bool Context::string_buffer(const char *str, size_t size, memcached_string_t &string_) {
  if (memcached_string_set(_string_buffer, str, size)) {
    string_.c_str = memcached_string_value(_string_buffer);
    string_.size = memcached_string_length(_string_buffer);

    return true;
  }

  return false;
}

bool Context::set_hash(memcached_hash_t hash) {
  if (_has_hash) {
    return false;
  }

  if ((memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_HASH, hash)) != MEMCACHED_SUCCESS) {
    return false;
  }

  return _has_hash = true;
}
