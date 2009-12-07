# libmemcached
# Copyright (C) 2008 Brian Aker, Monty Taylor
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in this directory for full text.

AC_PREREQ(2.59)
AC_INIT([libmemcached],[0.35],[http://tangent.org/552/libmemcached.html])
AC_CONFIG_SRCDIR([libmemcached/memcached.c])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

PANDORA_CANONICAL_TARGET

AC_SEARCH_LIBS(getopt_long, gnugetopt)
AC_SEARCH_LIBS(gethostbyname, nsl)

AC_CHECK_FUNCS_ONCE([getline])


PANDORA_HAVE_LIBEVENT
PANDORA_REQUIRE_PTHREAD
PANDORA_CXX_DEMANGLE

dnl Specialty checks
DETECT_BYTEORDER
ENABLE_UTILLIB
SETSOCKOPT_SANITY
ENABLE_HSIEH_HASH
REQUIRE_POD2MAN
PROTOCOL_BINARY_TEST
WITH_MEMCACHED
ENABLE_DEPRECATED
PANDORA_HAVE_LIBINNODB
PANDORA_PRINT_CALLSTACK

AC_CONFIG_FILES([
  Makefile
  clients/Makefile
  tests/Makefile
  docs/Makefile
  example/Makefile
  libmemcached/Makefile
  libmemcached/memcached_configure.h
  support/Makefile
  support/libmemcached.pc
  support/libmemcached.spec
  support/libmemcached-fc.spec
  ])
AC_OUTPUT

echo "---"
echo "Configuration summary for $PACKAGE_NAME version $VERSION"
echo ""
echo "   * Installation prefix:       $prefix"
echo "   * System type:               $host_vendor-$host_os"
echo "   * Host CPU:                  $host_cpu"
echo "   * C Compiler:                $CC_VERSION"
echo "   * Assertions enabled:        $ac_cv_assert"
echo "   * Debug enabled:             $with_debug"
echo "   * Warnings as failure:       $ac_cv_warnings_as_errors"
echo ""
echo "---"

