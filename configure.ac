# Gearman server and library
# Copyright (C) 2008 Brian Aker
# All rights reserved.
#
# Use and distribution licensed under the BSD license.  See
# the COPYING file in this directory for full text.

AC_PREREQ(2.59)
AC_INIT([libmemcached],[0.30],[http://tangent.org/552/libmemcached.html])
AC_CONFIG_SRCDIR([libmemcached/memcached.c])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(libmemcached/libmemcached_config.h)

#shared library versioning
MEMCACHED_LIBRARY_VERSION=2:0:0
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
AC_SUBST(MEMCACHED_LIBRARY_VERSION)
MEMCACHEDUTIL_LIBRARY_VERSION=0:0:0
AC_SUBST(MEMCACHEDUTIL_LIBRARY_VERSION)

# Setting CFLAGS here prevents AC_CANONICAL_TARGET from injecting them
SAVE_CFLAGS=${CFLAGS}
SAVE_CXXFLAGS=${CXXFLAGS}
CFLAGS=
CXXFLAGS=

AC_CANONICAL_TARGET

CFLAGS=${SAVE_CFLAGS}
CXXFLAGS=${SAVE_CXXFLAGS}

AM_INIT_AUTOMAKE([-Wall -Werror nostdinc subdir-objects])

if test "x${enable_dependency_tracking}" = "x"
then
  enable_dependency_tracking=yes
fi

if test -d "${srcdir}/.bzr"
then
  building_from_bzr=yes
else
  building_from_bzr=no
fi

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_STDC
AC_PROG_CXX

ACX_USE_SYSTEM_EXTENSIONS

AC_PROG_CPP
AM_PROG_CC_C_O

AC_C_BIGENDIAN
AC_C_CONST
AC_HEADER_TIME
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CHECK_DECL([__SUNPRO_C], [SUNCC="yes"], [SUNCC="no"])

# We use libtool
AC_PROG_LIBTOOL

AC_MSG_CHECKING("C Compiler version")
if test "$GCC" = "yes"
then
  CC_VERSION=`$CC --version | sed 1q`
elif test "$SUNCC" = "yes"
then
  CC_VERSION=`$CC -V 2>&1 | sed 1q`
else
  CC_VERSION=""
fi
AC_MSG_RESULT("$CC_VERSION")
AC_SUBST(CC_VERSION)

# libmemcached versioning when linked with GNU ld.
if test "$lt_cv_prog_gnu_ld" = "yes"
then
    LD_VERSION_SCRIPT="-Wl,--version-script=\$(top_srcdir)/libmemcached/libmemcached.ver"
    LD_UTIL_VERSION_SCRIPT="-Wl,--version-script=\$(top_srcdir)/libmemcachedutil/libmemcachedutil.ver"
fi
AC_SUBST(LD_VERSION_SCRIPT)
AC_SUBST(LD_UTIL_VERSION_SCRIPT)

# Use the C99 enabling flag found earlier
AS_IF([test "$ac_cv_prog_cc_c99" != "no"],
      [CFLAGS="$ac_cv_prog_cc_c99 ${CFLAGS}"])

# Build optimized or debug version ?
# First check for gcc and g++
if test "$GCC" = "yes"
then
  CFLAGS="-ggdb3 ${CFLAGS}"
  DEBUG_CFLAGS="-O0"
  OPTIMIZE_CFLAGS="-O3"
fi
if test "x$SUNCC" = "xyes"
then
   CFLAGS="-g -mt ${CFLAGS}"
   CXXFLAGS="-g -mt -xlang=c99 ${CXXFLAGS}"
   OPTIMIZE_CFLAGS="-xO4 -xlibmil -xdepend -Xa -xstrconst"
fi

ENABLE_64BIT

#--------------------------------------------------------------------
# Check for libpthread
#--------------------------------------------------------------------

ACX_PTHREAD(,AC_MSG_ERROR(could not find libpthread))
LIBS="${PTHREAD_LIBS} ${LIBS}"
CFLAGS="${PTHREAD_CFLAGS} ${CFLAGS}"
CC="$PTHREAD_CC"


AC_SEARCH_LIBS(getopt_long, gnugetopt)
AC_SEARCH_LIBS(socket, socket)
AC_SEARCH_LIBS(gethostbyname, nsl)

save_LIBS="$LIBS"
LIBS="$LIBS -lm"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM(
    [[
#include <stdlib.h>
    ]],[[
      float f= floorf((float) 1.0);
    ]],
    [LIBM="-lm"],[LIBM=""])])
    
AC_SUBST(LIBM)
LIBS="$save_LIBS"

AC_SEARCH_LIBS(floorf, m)

dnl Specialty checks
DETECT_BYTEORDER
ENABLE_UTILLIB
ENABLE_DTRACE
SETSOCKOPT_SANITY
ENABLE_HSIEH_HASH
REQUIRE_POD2MAN
PROTOCOL_BINARY_TEST
WITH_MEMCACHED

dnl TODO: Remove this define once we are using 2.61 across the board.
# AX_HEADER_ASSERT
# ----------------
# Check whether to enable assertions.
AC_DEFUN([AX_HEADER_ASSERT],
[
  AC_MSG_CHECKING([whether to enable assertions])
  AC_ARG_ENABLE([assert],
    [AS_HELP_STRING([--disable-assert],
       [Turn off assertions])],
    [ac_cv_assert="no"],
    [ac_cv_assert="yes"])
  AC_MSG_RESULT([$ac_cv_assert])
])
AX_HEADER_ASSERT


AC_ARG_WITH([debug],
    [AS_HELP_STRING([--with-debug],
       [Add debug code/turns off optimizations (yes|no) @<:@default=no@:>@])],
    [with_debug=$withval],
    [with_debug=no])
if test "$with_debug" = "yes"
then
  # Debugging. No optimization.
  CFLAGS="${DEBUG_CFLAGS} -DDEBUG ${CFLAGS}"
else
  # Optimized version. No debug
  CFLAGS="${OPTIMIZE_CFLAGS} ${CFLAGS}"
fi

AC_ARG_ENABLE([profiling],
    [AS_HELP_STRING([--enable-profiling],
       [Toggle profiling @<:@default=off@:>@])],
    [ac_profiling="$enableval"],
    [ac_profiling="no"])

AC_ARG_ENABLE([coverage],
    [AS_HELP_STRING([--enable-coverage],
       [Toggle coverage @<:@default=off@:>@])],
    [ac_coverage="$enableval"],
    [ac_coverage="no"])

dnl We can't do this warning, so turn off
W_STRICT_ALIASING="-Wno-strict-aliasing"

if test "$GCC" = "yes"
then

  AC_CACHE_CHECK([whether it is safe to use -fdiagnostics-show-option],
    [ac_cv_safe_to_use_fdiagnostics_show_option_],
    [save_CFLAGS="$CFLAGS"
     CFLAGS="-fdiagnostics-show-option ${CFLAGS}"
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([],[])],
       [ac_cv_safe_to_use_fdiagnostics_show_option_=yes],
       [ac_cv_safe_to_use_fdiagnostics_show_option_=no])
     CFLAGS="$save_CFLAGS"])

  AS_IF([test "$ac_cv_safe_to_use_fdiagnostics_show_option_" = "yes"],
        [
          F_DIAGNOSTICS_SHOW_OPTION="-fdiagnostics-show-option"
        ])


  AC_CACHE_CHECK([whether it is safe to use -Wconversion],
    [ac_cv_safe_to_use_wconversion_],
    [save_CFLAGS="$CFLAGS"
     CFLAGS="-Werror -Wconversion ${CFLAGS}"
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([[
#include <stdbool.h>
void foo(bool a)
{
  (void)a;
}
       ]],[[
foo(0);
       ]])],
       [ac_cv_safe_to_use_wconversion_=yes],
       [ac_cv_safe_to_use_wconversion_=no])
     CFLAGS="$save_CFLAGS"])
  AS_IF([test "$ac_cv_safe_to_use_wconversion_" = "yes"],
    [W_CONVERSION="-Wconversion"
    AC_CACHE_CHECK([whether it is safe to use -Wconversion with htons],
      [ac_cv_safe_to_use_Wconversion_],
      [save_CFLAGS="$CFLAGS"
       CFLAGS="-Werror -Wconversion ${CFLAGS}"
       AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM(
           [[
#include <netinet/in.h>
           ]],[[
uint16_t x= htons(80);
           ]])],
         [ac_cv_safe_to_use_Wconversion_=yes],
         [ac_cv_safe_to_use_Wconversion_=no])
       CFLAGS="$save_CFLAGS"])

    AS_IF([test "$ac_cv_safe_to_use_Wconversion_" = "no"],
          [
            NO_CONVERSION="-Wno-conversion"
          ])
    ])

  W_STRICT_ALIASING="-Wno-strict-aliasing"

  AS_IF([test "$ac_profiling" = "yes"],
        [CC_PROFILING="-pg"])

  AS_IF([test "$ac_coverage" = "yes"],
        [CC_COVERAGE="-fprofile-arcs -ftest-coverage"])

  AS_IF([test "$building_from_bzr" = "yes"],
        [W_FAIL="-Werror"])

  BASE_WARNINGS="-pedantic -W -Wall -Wextra ${W_FAIL} -Wundef -Wshadow -Wmissing-declarations ${W_STRICT_ALIASING} ${F_DIAGNOSTICS_SHOW_OPTION} ${W_CONVERSION}"
  CC_WARNINGS="${BASE_WARNINGS} -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls -Wswitch-default -Wswitch-enum -Wcast-align -Wconversion"
  CXX_WARNINGS="${BASE_WARNINGS} -Woverloaded-virtual  -Wnon-virtual-dtor -Wctor-dtor-privacy  -Wold-style-cast -Weffc++ -Wno-long-long -Wconversion"


  AC_CACHE_CHECK([whether it is safe to use -Wlogical-op],
    [ac_cv_safe_to_use_Wlogical_op_],
    [save_CFLAGS="$CFLAGS"
     CFLAGS="-Wlogical-op"
     AC_COMPILE_IFELSE([
       AC_LANG_PROGRAM(
       [[
#include <stdio>
       ]], [[]])
    ],
    [ac_cv_safe_to_use_Wlogical_op_=yes],
    [ac_cv_safe_to_use_Wlogical_op_=no])
  CFLAGS="$save_CFLAGS"])
  AS_IF([test "$ac_cv_safe_to_use_Wlogical_op_" = "yes"],
        [CC_WARNINGS="${CC_WARNINGS} -Wlogical-op"])

  AC_CACHE_CHECK([whether it is safe to use -Wredundant-decls from C++],
    [ac_cv_safe_to_use_Wredundant_decls_],
    [AC_LANG_PUSH(C++)
     save_CXXFLAGS="${CXXFLAGS}"
     CXXFLAGS="${CXXFLAGS} ${W_FAIL} -Wredundant-decls"
     AC_COMPILE_IFELSE(
       [AC_LANG_PROGRAM([
template <typename E> struct C { void foo(); };
template <typename E> void C<E>::foo() { }
template <> void C<int>::foo();
          AC_INCLUDES_DEFAULT])],
        [ac_cv_safe_to_use_Wredundant_decls_=yes],
        [ac_cv_safe_to_use_Wredundant_decls_=no])
      CXXFLAGS="${save_CXXFLAGS}"
      AC_LANG_POP()])
  AS_IF([test "$ac_cv_safe_to_use_Wredundant_decls_" = "yes"],
        [CXX_WARNINGS="${CXX_WARNINGS} -Wredundant-decls"],
        [CXX_WARNINGS="${CXX_WARNINGS} -Wno-redundant-decls"])

  NO_REDUNDANT_DECLS="-Wno-redundant-decls"
fi
if test "$SUNCC" = "yes"
then

  AS_IF([test "$ac_profiling" = "yes"],
        [CC_PROFILING="-xinstrument=datarace"])

  AS_IF([test "$building_from_bzr" = "yes"],
        [W_FAIL="-errwarn=%all"])

  AC_CACHE_CHECK([whether E_PASTE_RESULT_NOT_TOKEN is usable],
    [ac_cv_paste_result],
    [
      save_CFLAGS="${CFLAGS}"
      CFLAGS="-errwarn=%all -erroff=E_PASTE_RESULT_NOT_TOKEN ${CFLAGS}"
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([
          AC_INCLUDES_DEFAULT
        ],[
          int x= 0;])],
        [ac_cv_paste_result=yes],
        [ac_cv_paste_result=no])
      CFLAGS="${save_CFLAGS}"
    ])
  AS_IF([test $ac_cv_paste_result = yes],
    [W_PASTE_RESULT=",E_PASTE_RESULT_NOT_TOKEN"])


  CC_WARNINGS="-v -errtags=yes ${W_FAIL} -erroff=E_INTEGER_OVERFLOW_DETECTED${W_PASTE_RESULT}"
  CXX_WARNINGS="+w +w2 -xwe -xport64 -errtags=yes ${W_FAIL}"
fi

AC_SUBST(NO_CONVERSION)
AC_SUBST(NO_REDUNDANT_DECLS)

AM_CPPFLAGS="-I\$(top_srcdir) -I\$(top_builddir) ${CPPFLAGS}"
AM_CFLAGS="${CC_WARNINGS} ${CC_PROFILING} ${CC_COVERAGE} ${CFLAGS}"
AM_CXXFLAGS="${CXX_WARNING} ${CC_PROFILING} ${CC_COVERAGE} ${CXXFLAGS}"

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])

dnl We've collected the flags in AM_*FLAGS now, so blank these.
CFLAGS=""
CXXFLAGS=""
CPPFLAGS=""

AC_CONFIG_FILES([
  Makefile
  clients/Makefile
  tests/Makefile
  docs/Makefile
  libmemcached/Makefile
  libmemcachedutil/Makefile
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
echo "   * Warnings as failure:       $building_from_bzr"
echo ""
echo "---"

