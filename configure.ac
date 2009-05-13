AC_INIT(clients/memcat.c)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(libmemcached/libmemcached_config.h)

# Setting CFLAGS here prevents AC_CANONICAL_TARGET from injecting them
SAVE_CFLAGS=${CFLAGS}
SAVE_CXXFLAGS=${CXXFLAGS}
CFLAGS=
CXXFLAGS=

AC_CANONICAL_TARGET

CFLAGS=${SAVE_CFLAGS}
CXXFLAGS=${SAVE_CXXFLAGS}

MEMCACHED_LIBRARY_NAME=libmemcached 

#release versioning
MEMCACHED_MAJOR_VERSION=0
MEMCACHED_MINOR_VERSION=29
MEMCACHED_MICRO_VERSION=0

#API version
MEMCACHED_API_VERSION=1.0
AC_SUBST(MEMCACHED_API_VERSION)

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

PACKAGE=$MEMCACHED_LIBRARY_NAME
AC_SUBST(MEMCACHED_LIBRARY_NAME)

MEMCACHED_VERSION=$MEMCACHED_MAJOR_VERSION.$MEMCACHED_MINOR_VERSION.$MEMCACHED_MICRO_VERSION
MEMCACHED_RELEASE=$MEMCACHED_MAJOR_VERSION.$MEMCACHED_MINOR_VERSION
AC_SUBST(MEMCACHED_RELEASE)
AC_SUBST(MEMCACHED_VERSION)

VERSION=$MEMCACHED_RELEASE

AM_INIT_AUTOMAKE($PACKAG$, $VERSION, nostdinc no-define -Wall -Werror)
AC_USE_SYSTEM_EXTENSIONS

AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_SANITY_CHECK
LIBTOOL="$LIBTOOL --preserve-dup-deps"
AC_SUBST(LIBTOOL)dnl

AC_C_CONST
AC_HEADER_TIME
AC_TYPE_SIZE_T
AC_SEARCH_LIBS(getopt_long, gnugetopt)
AC_SEARCH_LIBS(socket, socket)
AC_SEARCH_LIBS(gethostbyname, nsl)
AC_SEARCH_LIBS(floorf, m)


sinclude(config/pod2man.m4)
sinclude(config/debug.m4)
sinclude(config/dtrace.m4)
sinclude(config/byteorder.m4)
sinclude(config/64bit.m4)
sinclude(config/protocol_binary.m4)
sinclude(config/memcached.m4)
sinclude(config/setsockopt.m4)
sinclude(config/hsieh.m4)
sinclude(config/util.m4)

dnl This is likely subverted by vpath builds. How do we find the original
dnl source dir in the configure step of a vpath build?
if test -d ".hg"
then
  building_from_hg=yes
else
  building_from_hg=no
fi

# We only support GCC and Sun's forte at the moment
if test "$GCC" = "yes"
then
  if test "$building_from_hg" = "yes"
  then
    CFLAGS="-Werror $CFLAGS"
    CXXFLAGS="-Werror $CXXFLAGS"
  fi

  CFLAGS="-W -std=iso9899:1999 -Wall -Wextra -Wno-strict-aliasing -pedantic -Wundef -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -O3 -ggdb $CFLAGS"
  CXXFLAGS="-W -Wall -Wextra -Wno-strict-aliasing -pedantic -Wundef -Woverloaded-virtual  -Wnon-virtual-dtor -Wctor-dtor-privacy -Wold-style-cast -Weffc++ -Wconversion -Wmissing-declarations -Wredundant-decls -O3 -ggdb $CXXFLAGS"

  if test "$ENABLE_DEBUG" = "yes"
  then
    CFLAGS="$CFLAGS -ggdb -DHAVE_DEBUG"
  fi
else
  CFLAGS="-Xa -xstrconst -mt -D_FORTEC_ -errfmt=error -errwarn -errshort=tags $CFLAGS"
  CXXFLAGS="+w +w2 -xwe -mt -D_FORTEC_ $CXXFLAGS"
  if test "$ENABLE_DEBUG" = "yes"
  then
    CFLAGS="$CFLAGS -DHAVE_DEBUG"
    CXXFLAGS="$CXXFLAGS -DHAVE_DEBUG"
  fi
fi


AM_CPPFLAGS="${CPPFLAGS}"
AM_CFLAGS="${CFLAGS}"
AM_CXXFLAGS="${CXXFLAGS}"

AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])
AC_SUBST([AM_CXXFLAGS])

dnl We've collected the flags in AM_*FLAGS now, so blank these.
CFLAGS=""
CXXFLAGS=""
CPPFLAGS=""

AC_OUTPUT(Makefile clients/Makefile tests/Makefile docs/Makefile libmemcached/Makefile libmemcachedutil/Makefile support/Makefile support/libmemcached.pc support/libmemcached.spec support/libmemcached-fc.spec)
