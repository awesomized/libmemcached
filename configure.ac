#!/usr/bin/env bash
# libmemcached
# Copyright (C) 2008 Brian Aker, Monty Taylor
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in this directory for full text.

AC_PREREQ(2.59)
AC_INIT([libmemcached],[0.42],[http://libmemcached.org/])
AC_CONFIG_SRCDIR([libmemcached/memcached.c])
AC_CONFIG_AUX_DIR(config)

PANDORA_CANONICAL_TARGET

#shared library versioning
MEMCACHED_UTIL_LIBRARY_VERSION=1:0:0
MEMCACHED_PROTOCAL_LIBRARY_VERSION=0:0:0
MEMCACHED_LIBRARY_VERSION=5:1:0
#                         | | |
#                  +------+ | +---+
#                  |        |     |
#                 current:revision:age
#                  |        |     |
#                  |        |     +- increment if interfaces have been added
#                  |        |        set to zero if interfaces have been removed or changed
#                  |        +- increment if source code has changed
#                  |           set to zero if current is incremented
#                  +- increment if interfaces have been added, removed or changed
AC_SUBST(MEMCACHED_UTIL_LIBRARY_VERSION)
AC_SUBST(MEMCACHED_PROTOCAL_LIBRARY_VERSION)
AC_SUBST(MEMCACHED_LIBRARY_VERSION)


HASHKIT_LIBRARY_VERSION=0:0:0
AC_SUBST(HASHKIT_LIBRARY_VERSION)

AC_SEARCH_LIBS(getopt_long, gnugetopt)
AC_SEARCH_LIBS(gethostbyname, nsl)

AC_CHECK_FUNCS([getline])

PANDORA_HAVE_LIBGTEST
PANDORA_HAVE_LIBEVENT
PANDORA_REQUIRE_PTHREAD
PANDORA_CXX_DEMANGLE

dnl Specialty checks
DETECT_BYTEORDER
ENABLE_UTILLIB
SETSOCKOPT_SANITY
ENABLE_HSIEH_HASH
REQUIRE_POD2MAN
REQUIRE_PODCHECKER
PROTOCOL_BINARY_TEST
WITH_MEMCACHED
ENABLE_DEPRECATED
PANDORA_HAVE_LIBINNODB
PANDORA_PRINT_CALLSTACK
PANDORA_HAVE_SASL

dnl The sasl functions should only be visible if we build with sasl support
AS_IF([test "x$ac_cv_sasl" = "xyes"],
      [LIBMEMCACHED_WITH_SASL_SUPPORT="#define LIBMEMCACHED_WITH_SASL_SUPPORT 1"])
AC_SUBST(LIBMEMCACHED_WITH_SASL_SUPPORT)

AC_CHECK_HEADERS([atomic.h])
AS_IF([test "x$ac_cv_header_atomic_h" = "xyes"],[
      AC_CHECK_FUNCS(atomic_add_64)
      AC_CHECK_FUNCS(atomic_add_32)
      AS_IF([test "x$ac_cv_func_atomic_add_64" = "xyes" -a "x$ac_cv_func_atomic_add_32" = "xyes"],[
            AC_DEFINE([USE_ATOMIC_H],
	              [1],
                      [Define to true if you want to use functions from atomic.h])])])

AC_CONFIG_FILES([
  Makefile
  docs/Makefile
  libhashkit/configure.h
  libmemcached/configure.h
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

case "$host_os" in
  *freebsd*)
    echo "*****"
    echo "*"
    echo "*  NOTE: You are on FreeBSD. BSD make will not work."
    echo "*"
    echo "*   use 'gmake' To build libmemcached"
    echo "*"
    echo "*****"
  ;;
esac

