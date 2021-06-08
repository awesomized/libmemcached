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

#ifdef __cplusplus
#  include <cassert>
#else
#  include <assert.h>
#endif // __cplusplus

#ifdef NDEBUG
#  define assert_msg(__expr, __mesg) \
    (void) (__expr); \
    (void) (__mesg);
#  define assert_vmsg(__expr, __mesg, ...) \
    (void) (__expr); \
    (void) (__mesg);
#else

#  ifdef _WIN32
#    include <malloc.h>
#  endif

#  ifdef __cplusplus
#    include <cstdarg>
#    include <cstdlib>
#    include <cstdio>
#  else
#    include <stdarg.h>
#    include <stdlib.h>
#    include <stdio.h>
#  endif

#  ifdef HAVE_ALLOCA_H
#    include <alloca.h>
#  endif

#  include "libmemcached/backtrace.hpp"

#  define assert_msg(__expr, __mesg) \
    do { \
      if (not(__expr)) { \
        fprintf(stderr, "\n%s:%d Assertion \"%s\" failed for function \"%s\" likely for %s\n", \
                __FILE__, __LINE__, #__expr, __func__, (#__mesg)); \
        custom_backtrace(); \
        abort(); \
      } \
    } while (0)

#  define assert_vmsg(__expr, __mesg, ...) \
    do { \
      if (not(__expr)) { \
        size_t ask = snprintf(0, 0, (__mesg), __VA_ARGS__); \
        ask++; \
        char *_error_message = (char *) alloca(sizeof(char) * ask); \
        size_t _error_message_size = snprintf(_error_message, ask, (__mesg), __VA_ARGS__); \
        fprintf(stderr, "\n%s:%d Assertion '%s' failed for function '%s' [ %.*s ]\n", __FILE__, \
                __LINE__, #__expr, __func__, int(_error_message_size), _error_message); \
        custom_backtrace(); \
        abort(); \
      } \
    } while (0)

#endif // NDEBUG
