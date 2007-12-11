AC_INIT(src/memcat.c)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(include/libmemcached_config.h)
AC_CANONICAL_SYSTEM

MEMCACHED_LIBRARY_NAME=libmemcached

#release versioning
MEMCACHED_MAJOR_VERSION=0
MEMCACHED_MINOR_VERSION=12
MEMCACHED_MICRO_VERSION=0

#API version
MEMCACHED_API_VERSION=1.0
AC_SUBST(MEMCACHED_API_VERSION)

#shared library versioning
MEMCACHED_LIBRARY_VERSION=2:2:1
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

PACKAGE=$MEMCACHED_LIBRARY_NAME
AC_SUBST(MEMCACHED_LIBRARY_NAME)

MEMCACHED_VERSION=$MEMCACHED_MAJOR_VERSION.$MEMCACHED_MINOR_VERSION.$MEMCACHED_MICRO_VERSION
MEMCACHED_RELEASE=$MEMCACHED_MAJOR_VERSION.$MEMCACHED_MINOR_VERSION
AC_SUBST(MEMCACHED_RELEASE)
AC_SUBST(MEMCACHED_VERSION)

VERSION=$MEMCACHED_RELEASE

AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_SANITY_CHECK
LIBTOOL="$LIBTOOL --preserve-dup-deps"
AC_SUBST(LIBTOOL)dnl
AC_LANG_CPLUSPLUS

sinclude(config/debug.m4)
sinclude(config/dtrace.m4)

# We only support GCC and Sun's forte at the moment
if test "$GCC" = "yes"
then
  if test "$ENABLE_DEBUG" = "yes"
  then
    CFLAGS="-Wall -ggdb -DHAVE_DEBUG"
  else
    CFLAGS="-Wall -O3"
  fi
else
  CFLAGS="-Xa -xstrconst -mt -D_FORTEC_ -fast -m64"
  LDFLAGS="-lsocket -lnsl"
  DTRACEFLAGS="-64"
fi

AC_C_CONST
AC_TYPE_SIZE_T
AC_OUTPUT(Makefile src/Makefile tests/Makefile docs/Makefile lib/Makefile include/Makefile support/Makefile support/libmemcached.pc support/libmemcached.spec)
