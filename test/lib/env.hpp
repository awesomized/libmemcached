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

#include "mem_config.h"

#include <cstdlib>

#if HAVE_SETENV
#  define SET_ENV_EX(n, k, v, overwrite) setenv(k, v, (overwrite))
#else // !HAVE_SETENV
#  define SET_ENV_EX(n, k, v, overwrite) \
    do { \
      static char n##_env[] = k "=" v; \
      if ((overwrite) || !getenv(k)) { \
        putenv(n##_env); \
      } \
    } while (false)
#endif

#define SET_ENV(symbolic_name, literal_env_var, literal_env_val) \
  SET_ENV_EX(symbolic_name, literal_env_var, literal_env_val, true)

static inline const char *getenv_else(const char *var, const char *defval) {
  auto val = getenv(var);
  if (val && *val) {
    return val;
  }
  return defval;
}
