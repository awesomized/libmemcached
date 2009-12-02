dnl  Copyright (C) 2009 Sun Microsystems
dnl This file is free software; Sun Microsystems
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl Check for all of the headers and libs that Drizzle needs. We check all
dnl of these for plugins too, to ensure that all of the appropriate defines
dnl are set.

AC_DEFUN([PANDORA_DRIZZLE_BUILD],[

  dnl We need to turn on our CXXFLAGS to make sure it shows up correctly
  save_CXXFLAGS="${CXXFLAGS}"
  CXXFLAGS="${CXXFLAGS} ${AM_CXXFLAGS}"
  PANDORA_CXX_STL_HASH
  CXXFLAGS="${save_CXXFLAGS}"

  PANDORA_CXX_CSTDINT
  PANDORA_CXX_CINTTYPES

  AC_STRUCT_TM

  AC_FUNC_ALLOCA
  AC_FUNC_UTIME_NULL
  AC_FUNC_VPRINTF

  PANDORA_WORKING_FDATASYNC

  AC_CHECK_FUNCS(\
    gethrtime \
    setupterm \
    backtrace \
    backtrace_symbols \
    backtrace_symbols_fd)

  AC_HEADER_STAT
  AC_HEADER_DIRENT
  AC_HEADER_STDC
  AC_HEADER_SYS_WAIT
  AC_HEADER_STDBOOL

  AC_CHECK_HEADERS(sys/fpu.h fpu_control.h ieeefp.h)
  AC_CHECK_HEADERS(select.h sys/select.h)
  AC_CHECK_HEADERS(utime.h sys/utime.h )
  AC_CHECK_HEADERS(synch.h sys/mman.h sys/socket.h)
  AC_CHECK_HEADERS(sched.h)
  AC_CHECK_HEADERS(sys/prctl.h)
  AC_CHECK_HEADERS(execinfo.h)
  AC_CHECK_HEADERS(locale.h)
  AC_CHECK_HEADERS(termcap.h termio.h termios.h asm/termbits.h)
  AC_CHECK_HEADERS(paths.h)

  
  #--------------------------------------------------------------------
  # Check for system libraries. Adds the library to $LIBS
  # and defines HAVE_LIBM etc
  #--------------------------------------------------------------------
  
  AC_CHECK_LIB(m, floor, [], AC_CHECK_LIB(m, __infinity))
  
  AC_CHECK_FUNC(setsockopt, [], [AC_CHECK_LIB(socket, setsockopt)])
  # This may get things to compile even if bind-8 is installed
  AC_CHECK_FUNC(bind, [], [AC_CHECK_LIB(bind, bind)])
  
  # For the sched_yield() function on Solaris
  AC_CHECK_FUNC(sched_yield, [],
    [AC_CHECK_LIB(posix4, [sched_yield],
      [AC_DEFINE(HAVE_SCHED_YIELD, 1, [Have sched_yield function]) LIBS="$LIBS -lposix4"])])
  
  AS_IF([test "$ac_cv_header_termio_h" = "no" -a "$ac_cv_header_termios_h" = "no"],[
    AC_CHECK_FUNC(gtty, [], [AC_CHECK_LIB(compat, gtty)])
  ])
  
  AC_CHECK_HEADERS([curses.h term.h],[],[],[[
    #ifdef HAVE_CURSES_H
    # include <curses.h>
    #endif
  ]])
  AC_CHECK_TYPES([ulong])

  AC_LANG_PUSH([C++])
  AC_CHECK_HEADERS(cxxabi.h)
  AC_CACHE_CHECK([checking for abi::__cxa_demangle], pandora_cv_cxa_demangle,
  [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <cxxabi.h>]], [[
    char *foo= 0; int bar= 0;
    foo= abi::__cxa_demangle(foo, foo, 0, &bar);
  ]])],[pandora_cv_cxa_demangle=yes],[pandora_cv_cxa_demangle=no])])
  AC_LANG_POP([])

  AS_IF([test "x$pandora_cv_cxa_demangle" = xyes],[
    AC_DEFINE(HAVE_ABI_CXA_DEMANGLE, 1,
              [Define to 1 if you have the `abi::__cxa_demangle' function.])
  ])


])
