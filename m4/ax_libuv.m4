# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIBUV([ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]]) 
#
# DESCRIPTION
#
#   libuv library
#
# LICENSE
#
#   Copyright (c) 2012 Brian Aker <brian@tangent.org>
#   Copyright (c) 2014 Andrew Hutchings <andrew@linuxjedi.co.uk>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1
 
AC_DEFUN([AX_LIBUV],
         [AC_PREREQ([2.63])dnl
         AC_CACHE_CHECK([test for a working libuv],[ax_cv_libuv],
           [AX_SAVE_FLAGS
           LIBS="-luv $LIBS"
           AC_LANG_PUSH([C])
           AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <uv.h>
               ]],[[
               uv_loop_t *loop;
               loop = uv_loop_new();
               ]])],
             [ax_cv_libuv=yes],
             [ax_cv_libuv=no],
             [AC_MSG_WARN([test program execution failed])])
           AC_LANG_POP
           AX_RESTORE_FLAGS
           ])

         AS_IF([test "x$ax_cv_libuv" = "xyes"],
             [AC_SUBST([LIBUV_LIB],[-luv])
             AC_DEFINE([HAVE_LIBUV],[1],[Define if uv_loop_new is present in uv.h.])],
             [AC_DEFINE([HAVE_LIBUV],[0],[Define if uv_loop_new is present in uv.h.])])

         AM_CONDITIONAL(HAVE_LIBUV, test "x$ax_cv_libuv" = "xyes")
# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
         AS_IF([test "x$ax_cv_libuv" = xyes],
             [$1],
             [$2])
         ])
