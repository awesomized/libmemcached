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

#shared library versioning
MEMCACHED_LIBRARY_VERSION=3:0:0
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
MEMCACHEDPROTOCOL_LIBRARY_VERSION=0:0:0
AC_SUBST(MEMCACHEDPROTOCOL_LIBRARY_VERSION)


# libmemcached versioning when linked with GNU ld.
if test "$lt_cv_prog_gnu_ld" = "yes"
then
    LD_VERSION_SCRIPT="-Wl,--version-script=\$(top_srcdir)/libmemcached/libmemcached.ver"
    LD_UTIL_VERSION_SCRIPT="-Wl,--version-script=\$(top_srcdir)/libmemcached/util/libmemcachedutil.ver"
    LD_PROTOCOL_VERSION_SCRIPT="-Wl,--version-script=\$(top_srcdir)/libmemcached/protocol/libmemcachedprotocol.ver"
fi
AC_SUBST(LD_VERSION_SCRIPT)
AC_SUBST(LD_UTIL_VERSION_SCRIPT)
AC_SUBST(LD_PROTOCOL_VERSION_SCRIPT)



#--------------------------------------------------------------------
# Check for libevent
#--------------------------------------------------------------------
trylibeventdir=""
AC_ARG_WITH(libevent,
       [  --with-libevent=PATH     Specify path to libevent installation ],
       [
                if test "x$withval" != "xno" ; then
                        trylibeventdir=$withval
                fi
       ]
)

dnl ------------------------------------------------------
dnl libevent detection.  swiped from Tor.  modified a bit.

LIBEVENT_URL=http://www.monkey.org/~provos/libevent/

AC_CACHE_CHECK([for libevent directory], ac_cv_libevent_dir, [
  saved_LIBS="$LIBS"
  saved_LDFLAGS="$LDFLAGS"
  saved_CPPFLAGS="$CPPFLAGS"
  le_found=no
  for ledir in $trylibeventdir "" $prefix /usr/local ; do
    LDFLAGS="$saved_LDFLAGS"
    LIBS="$saved_LIBS -levent"

    # Skip the directory if it isn't there.
    if test ! -z "$ledir" -a ! -d "$ledir" ; then
       continue;
    fi
    if test ! -z "$ledir" ; then
      if test -d "$ledir/lib" ; then
        LDFLAGS="-L$ledir/lib $LDFLAGS"
      else
        LDFLAGS="-L$ledir $LDFLAGS"
      fi
      if test -d "$ledir/include" ; then
        CPPFLAGS="-I$ledir/include $CPPFLAGS"
      else
        CPPFLAGS="-I$ledir $CPPFLAGS"
      fi
    fi
    # Can I compile and link it?
    AC_TRY_LINK([#include <sys/time.h>
#include <sys/types.h>
#include <event.h>], [ event_init(); ],
       [ libevent_linked=yes ], [ libevent_linked=no ])
    if test $libevent_linked = yes; then
       if test ! -z "$ledir" ; then
         ac_cv_libevent_dir=$ledir
       else
        ac_cv_libevent_dir="(system)"
       fi
       le_found=yes
       break
    fi
  done
  LIBS="$saved_LIBS"
  LDFLAGS="$saved_LDFLAGS"
  CPPFLAGS="$saved_CPPFLAGS"
  if test $le_found = no ; then
    AC_MSG_ERROR([libevent is required.  You can get it from $LIBEVENT_URL

      If it's already installed, specify its path using --with-libevent=/dir/
])
  fi
])
LIBS="$LIBS -levent"
if test $ac_cv_libevent_dir != "(system)"; then
  if test -d "$ac_cv_libevent_dir/lib" ; then
    LDFLAGS="-L$ac_cv_libevent_dir/lib $LDFLAGS"
    le_libdir="$ac_cv_libevent_dir/lib"
  else
    LDFLAGS="-L$ac_cv_libevent_dir $LDFLAGS"
    le_libdir="$ac_cv_libevent_dir"
  fi
  if test -d "$ac_cv_libevent_dir/include" ; then
    CPPFLAGS="-I$ac_cv_libevent_dir/include $CPPFLAGS"
  else
    CPPFLAGS="-I$ac_cv_libevent_dir $CPPFLAGS"
  fi
fi


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
SETSOCKOPT_SANITY
ENABLE_HSIEH_HASH
REQUIRE_POD2MAN
PROTOCOL_BINARY_TEST
WITH_MEMCACHED
ENABLE_DEPRECATED
PANDORA_HAVE_LIBINNODB

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

