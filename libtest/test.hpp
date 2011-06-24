/* uTest
 * Copyright (C) 2011 Data Differential, http://datadifferential.com/
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/*
  Structures for generic tests.
*/

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <arpa/inet.h>

#include <libtest/visibility.h>

#include <libtest/error.h>
#include <libtest/callbacks.h>
#include <libtest/test.h>
#include <libtest/strerror.h>
#include <libtest/core.h>
#include <libtest/runner.h>
#include <libtest/stats.h>
#include <libtest/collection.h>
#include <libtest/framework.h>
#include <libtest/get.h>

#pragma once

LIBTEST_API
  in_port_t default_port();

LIBTEST_API
  void set_default_port(in_port_t port);

#ifdef __cplusplus
#define test_literal_param(X) (X), (static_cast<size_t>((sizeof(X) - 1)))
#else
#define test_literal_param(X) (X), ((size_t)((sizeof(X) - 1)))
#endif

#define test_string_make_from_cstr(X) (X), ((X) ? strlen(X) : 0)
