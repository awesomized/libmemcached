# ===========================================================================
# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_UUID
#   AX_UUID_GENERATE_TIME_SAFE
#
# DESCRIPTION
#
#   Check for uuid, and uuid_generate_time_safe support.
#
# LICENSE
#
#   Copyright (c) 2012 Brian Aker <brian@tangent.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 4

AC_DEFUN([AX_UUID], [
    AC_CHECK_HEADER([uuid/uuid.h], [
      AC_CACHE_CHECK([check to see if -luuid is not needed], [ax_cv_libuuid_is_required], [
        AC_LANG_PUSH([C])
        AC_RUN_IFELSE([
          AC_LANG_PROGRAM([#include <uuid/uuid.h>], [
            uuid_t out;
            uuid_generate(out);
            ])],
          [ax_cv_libuuid_is_required=no],
          [ax_cv_libuuid_is_required=maybe],
          [AC_MSG_WARN([test program execution failed])])
        AC_LANG_POP
        ])

      AS_IF([test "$ax_cv_libuuid_is_required" = maybe], [
        AC_CACHE_CHECK([check to see if -luuid is needed], [ax_cv_libuuid_works], [
          save_LIBS="$LIBS"
          LIBS="-luuid $LIBS"
          AC_LANG_PUSH([C])
          AC_RUN_IFELSE([
            AC_LANG_PROGRAM([#include <uuid/uuid.h>], [
              uuid_t out;
              uuid_generate(out);
              ])],
            [ax_cv_libuuid_works=yes],
            [ax_cv_libuuid_works=no],
            [AC_MSG_WARN([test program execution failed])])
          AC_LANG_POP
          LIBS="$save_LIBS"
          ])
        AS_IF([test "$ax_cv_libuuid_works" = yes], [
          AC_SUBST([LIBUUID_LDFLAGS],[-luuid])])
        ])

      AS_IF([test "$ax_cv_libuuid_is_required" = no], [UUID_UUID_H=yes])
      AS_IF([test "$ax_cv_libuuid_works" = yes], [UUID_UUID_H=yes])
      ])

  AS_IF([test "$UUID_UUID_H" = yes], [
      AC_DEFINE([HAVE_UUID_UUID_H], [1], [Have uuid/uuid.h])
      ],[
      AC_DEFINE([HAVE_UUID_UUID_H], [0], [Have uuid/uuid.h])
      ])
  ])

  AC_DEFUN([AX_UUID_GENERATE_TIME_SAFE], [
      AC_REQUIRE([AX_UUID])
      AC_CACHE_CHECK([for uuid_generate_time_safe], [ax_cv_uuid_generate_time_safe], [
        save_LIBS="$LIBS"
        LIBS="$LIBUUID_LDFLAGS $LIBS"
        AC_LANG_PUSH([C])
        AC_RUN_IFELSE([
          AC_LANG_PROGRAM([#include <uuid/uuid.h>], [
            uuid_t out;
            uuid_generate_time_safe(out);
            ])],
          [ax_cv_uuid_generate_time_safe=yes],
          [ax_cv_uuid_generate_time_safe=no],
          [AC_MSG_WARN([test program execution failed])])
        AC_LANG_POP
        LIBS="$save_LIBS"
        ])

      AS_IF([test "$ax_cv_uuid_generate_time_safe" = yes],[
        AC_DEFINE([HAVE_UUID_GENERATE_TIME_SAFE],[1],[Define if uuid_generate_time_safe is present in uuid/uuid.h.])],[
        AC_DEFINE([HAVE_UUID_GENERATE_TIME_SAFE],[0],[Define if uuid_generate_time_safe is present in uuid/uuid.h.])
        ])
      ])
