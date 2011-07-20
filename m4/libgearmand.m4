AC_DEFUN([_WITH_LIBGEARMAN],
  [ AC_ARG_ENABLE([libgearman],
   [AS_HELP_STRING([--disable-libgearman],
     [Build with libgearman support @<:@default=on@:>@])],
     [ac_enable_libgearman="$enableval"],
     [ac_enable_libgearman="yes"])

     AS_IF([test "x$ac_enable_libgearman" = "xyes"],
       [ PKG_CHECK_MODULES([libgearman], [ libgearman >= 0.24 ], 
			   [AC_DEFINE([HAVE_LIBGEARMAN], [ 1 ], [Enable libgearman support])],
			   [ac_enable_libgearman="no"] )])

     AM_CONDITIONAL(HAVE_LIBGEARMAN, test "x${ac_enable_libgearman}" = "xyes")
     ])

AC_DEFUN([WITH_LIBGEARMAN], [ AC_REQUIRE([_WITH_LIBGEARMAN]) ])
