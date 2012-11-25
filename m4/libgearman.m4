# serial 1
AC_DEFUN([CHECK_FOR_LIBGEARMAND],
    [AX_CHECK_LIBRARY([LIBGEARMAN],[libgearman/gearman.h],[gearman], 
      [ax_check_for_libgearman=yes],
      [ax_check_for_libgearman=no])
    AS_IF([test "$ax_check_for_libgearman" = xyes],
      [AC_DEFINE([HAVE_LIBGEARMAN],[1],[Enables libgearman Support])],
      [AC_DEFINE([HAVE_LIBGEARMAN],[0],[Enables libgearman Support])])
    ])
