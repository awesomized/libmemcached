dnl -*- mode: m4; c-basic-offset: 2; indent-tabs-mode: nil; -*-
dnl vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
dnl   
dnl pandora-build: A pedantic build system
dnl Copyright (C) 2009 Sun Microsystems, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl
dnl From Monty Taylor


dnl --------------------------------------------------------------------
dnl  Check for libpthread
dnl --------------------------------------------------------------------

AC_DEFUN([_PANDORA_SEARCH_PTHREAD],[
  ACX_PTHREAD
  LIBS="${PTHREAD_LIBS} ${LIBS}"
  AM_CFLAGS="${PTHREAD_CFLAGS} ${AM_CFLAGS}"
  AM_CXXFLAGS="${PTHREAD_CFLAGS} ${AM_CXXFLAGS}"
  CC="$PTHREAD_CC"
])


AC_DEFUN([PANDORA_HAVE_PTHREAD],[
  AC_REQUIRE([_PANDORA_SEARCH_PTHREAD])
])

AC_DEFUN([PANDORA_REQUIRE_PTHREAD],[
  AC_REQUIRE([PANDORA_HAVE_PTHREAD])
  AS_IF([test "x$acx_pthread_ok" != "xyes"],[
    AC_MSG_ERROR(could not find libpthread)])
])
