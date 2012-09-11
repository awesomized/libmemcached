# ===========================================================================
# http://
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

#serial 3

AC_DEFUN([AX_UUID], [
         AC_CHECK_HEADER([uuid/uuid.h], [
                         AC_CACHE_CHECK([check to see if we need -luuid], [ax_cv_libuuid_is_required], [
                                        save_LDFLAGS="$LDFLAGS"
                                        LDFLAGS="-luuid"
                                        AC_LANG_PUSH([C])
                                        AC_LINK_IFELSE([
                                                       AC_LANG_PROGRAM([#include <uuid/uuid.h>], [
                                                                       uuid_t out;
                                                                       uuid_generate(out);
                                                                       ])],
                                                       [ax_cv_libuuid_is_required=yes],
                                                       [])
                                        AC_LANG_POP
                                        LDFLAGS="$save_LDFLAGS"
                                        ])
                         AS_IF([test x"ax_cv_libuuid_is_required" = xyes], [
                               LIBUUID_LDFLAGS="-luuid"
                               UUID_UUID_H=yes
                               ],[
                               AC_CACHE_CHECK([check to see if -luuid is not needed], [ax_cv_libuuid_is_not_required], [
                                              AC_LANG_PUSH([C])
                                              AC_COMPILE_IFELSE([
                                                                AC_LANG_PROGRAM([#include <uuid/uuid.h>], [
                                                                                uuid_t out;
                                                                                uuid_generate(out);
                                                                                ])],
                                                                [ax_cv_libuuid_is_not_required=yes],
                                                                [ax_cv_libuuid_is_not_required=no])
                                              AC_LANG_POP
                                              ])
                               AS_IF([test x"ax_cv_libuuid_is_not_required" = xyes], [UUID_UUID_H=yes], [UUID_UUID_H=])
                               ])
                         ], [UUID_UUID_H=])

AS_IF([test x"$UUID_UUID_H" = xyes], [
      AC_DEFINE([HAVE_UUID_UUID_H], [ 1 ], [Have uuid/uuid.h])
      ],[
      AC_DEFINE([HAVE_UUID_UUID_H], [ 0 ], [Have uuid/uuid.h])
      ])
])

AC_DEFUN([AX_UUID_GENERATE_TIME_SAFE], [
  AC_REQUIRE([AX_UUID])
  AC_CACHE_CHECK([for uuid_generate_time_safe], [ax_cv_uuid_generate_time_safe], [
        save_LDFLAGS="$LDFLAGS"
        LDFLAGS=$LIBUUID_LDFLAGS
        AC_LANG_PUSH([C])
        AC_LINK_IFELSE([
            AC_LANG_PROGRAM([#include <uuid/uuid.h>], [
                            uuid_t out;
                            uuid_generate_time_safe(out);
                            ])],
            [ax_cv_uuid_generate_time_safe=yes],
            [ax_cv_uuid_generate_time_safe=no])
          AC_LANG_POP
          LDFLAGS="$save_LDFLAGS"
          ])

  AS_IF([test "$ax_cv_uuid_generate_time_safe" = yes],[
    AC_DEFINE(HAVE_UUID_GENERATE_TIME_SAFE,[1],[Define if uuid_generate_time_safe is present in uuid/uuid.h.])],[
    AC_DEFINE(HAVE_UUID_GENERATE_TIME_SAFE,[0],[Define if uuid_generate_time_safe is present in uuid/uuid.h.])
  ])
])
