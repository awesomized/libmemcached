dnl  Copyright (C) 2009 Sun Microsystems
dnl This file is free software; Sun Microsystems
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([_PANDORA_SEARCH_LIBPQ],[
  AC_REQUIRE([AC_LIB_PREFIX])

  dnl --------------------------------------------------------------------
  dnl  Check for libpq
  dnl --------------------------------------------------------------------

  AC_LIB_HAVE_LINKFLAGS(pq,,[
    #include <libpq-fe.h>
  ], [
    PGconn *conn;
    conn = PQconnectdb(NULL);
  ])
  
  AM_CONDITIONAL(HAVE_LIBPQ, [test "x${ac_cv_libpq}" = "xyes"])
])

AC_DEFUN([PANDORA_HAVE_LIBPQ],[
  AC_REQUIRE([_PANDORA_SEARCH_LIBPQ])
])

AC_DEFUN([PANDORA_REQUIRE_LIBPQ],[
  AC_REQUIRE([PANDORA_HAVE_LIBPQ])
  AS_IF([test x$ac_cv_libpq = xno],
      AC_MSG_ERROR([libpq is required for ${PACKAGE}]))
])
