dnl  Copyright (C) 2011 Brian Aker (brian@tangent.org)

AC_DEFUN([_SEARCH_LIBHIREDIS],[
  AC_REQUIRE([AX_CHECK_LIBRARY])

  AS_IF([test "x$ac_enable_hires" = "xyes"],[
        AX_CHECK_LIBRARY([LIBHIREDIS], [hiredis/hiredis.h], [hiredis],
                         [
                         LIBHIREDIS_LDFLAGS="-lhiredis"
                         AC_DEFINE([HAVE_HIREDIS], [1], [If Hiredis available])
                         ],
                         [
                         AC_DEFINE([HAVE_HIREDIS], [0], [If Hiredis is available])
                         ac_enable_hires="no"
                         ])

        ],
        [
        AC_DEFINE([HAVE_HIREDIS], [0], [If Hiredis is available])
        ])

  AM_CONDITIONAL(HAVE_HIREDIS, [test "x$ac_cv_lib_hiredis_main" = "xyes"])
  ])

AC_DEFUN([AX_HAVE_LIBHIREDIS],[

  AC_ARG_ENABLE([hires],
    [AS_HELP_STRING([--disable-hires],
      [Build with hires support @<:@default=on@:>@])],
    [ac_enable_hires="$enableval"],
    [ac_enable_hires="yes"])

  _SEARCH_LIBHIREDIS
])
