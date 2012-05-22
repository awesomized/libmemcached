#!/usr/bin/env bash
# LibmemcacheD
# Copyright (C) 2011-2012 Data Differential, http://datadifferential.com/
# Copyright (C) 2006-2010 Brian Aker, Monty Taylor, Trond Norbye
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in this directory for full text.

AC_INIT([libmemcached],[1.0.8],[http://libmemcached.org/])

AC_CONFIG_AUX_DIR(config)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_CANONICAL_HOST
AC_CANONICAL_BUILD

AC_PREREQ([2.61])

AC_CONFIG_MACRO_DIR(m4)

AC_CONFIG_HEADERS([config.h:config.in])dnl Keep filename to 8.3 for MS-DOS.

AC_CONFIG_SRCDIR([libmemcached/memcached.cc])

PANDORA_CANONICAL_TARGET(no-vc-changelog)
AC_CHECK_PROGS([YACC], ['bison'], [:])
AC_CHECK_PROGS([LEX], ['flex'], [:])

#shared library versioning
MEMCACHED_UTIL_LIBRARY_VERSION=2:0:0
MEMCACHED_PROTOCAL_LIBRARY_VERSION=0:0:0
MEMCACHED_LIBRARY_VERSION=10:0:0
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


HASHKIT_LIBRARY_VERSION=2:0:0
AC_SUBST(HASHKIT_LIBRARY_VERSION)

LT_PREREQ([2.2])
LT_INIT()
LT_LANG([C++])
LT_LANG([C])
gl_VISIBILITY

AX_CXX_HEADER_STDCXX_98
  if test "$ax_cv_cxx_stdcxx_98" = no; then
    AC_MSG_ERROR([Your system lacks a new enough C++ compiler])
  fi

m4_include([m4/memcached.m4])
m4_include([m4/memcached_sasl.m4])
m4_include([m4/gearmand.m4])
m4_include([m4/libgearman.m4])
m4_include([memcached/version.m4])

AM_CONDITIONAL(BUILDING_LIBMEMCACHED, true)
AM_CONDITIONAL(HAVE_LIBMEMCACHED, false)
AM_CONDITIONAL(HAVE_LIBDRIZZLE, false)
AC_DEFINE([HAVE_LIBMEMCACHED], [1], [Enables libmemcached Support])

AM_CONDITIONAL(BUILDING_GEARMAN, false)

AC_SEARCH_LIBS(getopt_long, gnugetopt)
AC_SEARCH_LIBS(gethostbyname, nsl)

case "$target_os" in
  *linux*)
       AS_IF([test "x$GCC" = "xyes"],
             [
              LDFLAGS="$LDFLAGS -z relro -z now"
              ])
   ;;
   esac

dnl Specialty checks
AX_PTHREAD
AX_CXX_CINTTYPES
CONFIG_EXTRA
DETECT_BYTEORDER
SETSOCKOPT_SANITY
ENABLE_HSIEH_HASH
ENABLE_MURMUR_HASH
ENABLE_FNV64_HASH
ENABLE_MEMASLAP
PROTOCOL_BINARY_TEST
ENABLE_DEPRECATED

AC_CHECK_FUNCS([alarm])
AC_CHECK_FUNCS([dup2])
AC_CHECK_FUNCS([getline])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([memchr])
AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([pipe2])
AC_CHECK_FUNCS([select])
AC_CHECK_FUNCS([setenv])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strcasecmp])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([strtoul])
AC_CHECK_FUNCS([strtoull])

AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([inttypes.h])
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([math.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([stdarg.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([cxxabi.h], 
                 AC_DEFINE([HAVE_CXXABI_H], [1], [Have cxxabi.h]),
                 AC_DEFINE([HAVE_CXXABI_H], [0], [Have cxxabi.h]))

AC_CHECK_HEADERS([sys/sysctl.h])

AC_CHECK_HEADER(umem.h, [
   AC_DEFINE([HAVE_UMEM_H], 1,
         [Define this if you have umem.h])
   build_cache=no
], [build_cache=yes])

AM_CONDITIONAL([BUILD_CACHE], [test "x$build_cache" = "xyes"])

AX_COMPILER_VENDOR

AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE
AC_C_RESTRICT

AX_CXX_GCC_ABI_DEMANGLE

AX_SASL_CHECK

dnl
dnl The sasl functions should only be visible if we build with sasl support
dnl
AS_IF([test "x$ac_enable_sasl" = "xyes"], [
       [ LIBMEMCACHED_WITH_SASL_SUPPORT="#define LIBMEMCACHED_WITH_SASL_SUPPORT 1" ]
       ], [
           [ LIBMEMCACHED_WITH_SASL_SUPPORT="#define LIBMEMCACHED_WITH_SASL_SUPPORT 0" ]
           ])
AC_SUBST(LIBMEMCACHED_WITH_SASL_SUPPORT)

AX_CHECK_LIBRARY([LIBUUID], [uuid/uuid.h], [uuid], 
                 [
                  LIBUUID_LDFLAGS="-luuid"
                  AC_DEFINE([HAVE_LIBUUID], [ 1 ], [Have libuuid])
                  ],
                 [
                  AC_DEFINE([HAVE_LIBUUID], [ 0 ], [Have libuuid])
                  ])

AC_CHECK_LIB([rt], [clock_gettime], 
             [
              RT_LIB="-lrt"
              AC_SUBST(RT_LIB)
              AC_DEFINE([HAVE_LIBRT], [ 1 ], [Have clock_gettime])
              ], 
             [
              AC_DEFINE([HAVE_LIBRT], [ 0 ], [Have clock_gettime])
              ])

AC_CHECK_LIB([m], [floor])
AC_CHECK_FUNCS([sigignore])

AC_CHECK_HEADERS([atomic.h])
AS_IF([test "x$ac_cv_header_atomic_h" = "xyes"],[
      AC_CHECK_FUNCS(atomic_add_64)
      AC_CHECK_FUNCS(atomic_add_32)
      AS_IF([test "x$ac_cv_func_atomic_add_64" = "xyes" -a "x$ac_cv_func_atomic_add_32" = "xyes"],[
            AC_DEFINE([USE_ATOMIC_H],
	              [1],
                      [Define to true if you want to use functions from atomic.h])])])

AC_DEFINE([HAVE_LIBDRIZZLE], [0], [Support for libdrizzle])
AC_DEFINE([HAVE_DRIZZLED_BINARY], [0], [Support for DrizzleD])
AC_DEFINE([DRIZZLED_BINARY], [0], [Support for DrizzleD])
AC_DEFINE([GEARMAND_BLOBSLAP_WORKER], [0], [Support for Gearman Blobslap worker])
AC_DEFINE([HAVE_LIBPQ], [0], [Support for Postgres])
AC_DEFINE([HAVE_LIBCURL], [0], [Support for libcurl])

AC_DEFINE([HAVE_MEMCACHED_LIGHT_BINARY], [1], [Support for memcached_light])
AC_DEFINE([MEMCACHED_LIGHT_BINARY], ["example/memcached_light"], [Support for memcached_light])

dnl Check for the requirements for running memcached with less privileges
dnl than the default privilege set. On Solaris we need setppriv and priv.h
dnl If you want to add support for other platforms you should check for
dnl your requirements, define HAVE_DROP_PRIVILEGES, and make sure you add
dnl the source file containing the implementation into memcached_SOURCE
dnl in Makefile.am
AC_CHECK_FUNCS(setppriv, [
   AC_CHECK_HEADER(priv.h, [
      AC_DEFINE([HAVE_DROP_PRIVILEGES], 1,
         [Define this if you have an implementation of drop_privileges()])
      build_solaris_privs=yes
   ], [])
],[])

AC_CHECK_HEADERS_ONCE(winsock2.h poll.h sys/wait.h fnmatch.h)
AM_CONDITIONAL(BUILD_POLL, test "x$ac_cv_header_poll_h" = "xno")
AM_CONDITIONAL(BUILD_WIN32_WRAPPERS, test "x$ac_cv_header_winsock2_h" = "xyes")
AS_IF(test "x$ac_cv_header_winsock2_h" = "xyes",
      [AM_LDFLAGS="$AM_LDFLAGS -lws2_32"
       AM_CFLAGS="$AM_CFLAGS $NO_WERROR"
       AM_CXXFLAGS="$AM_CXXFLAGS $NO_WERROR"
      ])
SOCKET_SEND_FLAGS

AX_CHECK_LIBRARY([LIBEVENT], [event.h], [event], 
                 [
                  LIBEVENT_LDFLAGS="-levent"
                  ],
                 [
                  AC_MSG_WARN([Unable to find libevent])
                  ])
AM_CONDITIONAL(HAVE_LIBEVENT, test "x${ax_cv_have_LIBEVENT}" = "xyes")

dnl Check if we're a little-endian or a big-endian system, needed by hash code
AC_DEFUN([AC_C_ENDIAN],
[AC_CACHE_CHECK(for endianness, ac_cv_c_endian,
[
  AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([], [dnl
        long val = 1;
        char *c = (char *) &val;
        exit(*c == 1);
    ])
  ],[
    ac_cv_c_endian=big
  ],[
    ac_cv_c_endian=little
  ])
])
if test $ac_cv_c_endian = big; then
  AC_DEFINE(ENDIAN_BIG, 1, [machine is bigendian])
fi
if test $ac_cv_c_endian = little; then
  AC_DEFINE(ENDIAN_LITTLE, 1, [machine is littleendian])
fi
])

AC_C_ENDIAN

AC_CONFIG_FILES([
  Makefile
  docs/conf.py
  libhashkit-1.0/configure.h
  libmemcached-1.0/configure.h
  libmemcached-1.2/configure.h
  libmemcached-2.0/configure.h
  support/libmemcached.pc
  support/libmemcached.spec
  support/libmemcached-fc.spec
  libtest/version.h
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
echo "   * SASL support:              $ac_enable_sasl"
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

