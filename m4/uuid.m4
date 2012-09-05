# ===========================================================================
# http://
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_UUID
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

#serial 2

AC_DEFUN([AX_UUID_GENERATE_TIME_SAFE], [
    AC_CACHE_CHECK([check for uuid_generate_time_safe], [ax_cv_uuid_generate_time_safe], [
      save_LDFLAGS="$LDFLAGS"
      LDFLAGS="-luuid"
      AC_LANG_PUSH([C])
      AC_COMPILE_IFELSE([
        AC_LANG_PROGRAM([
#include <uuid/uuid.h>
      ], [
      uuid_t out;
      uuid_generate_time_safe(out);
      ]), [ax_cv_uuid_generate_time_safe=yes ], [ax_cv_uuid_generate_time_safe=no ]
      ])

    AC_LANG_POP
    LDFLAGS="$save_LDFLAGS"
    ])

  AS_IF([test "$ax_cv_uuid_generate_time_safe" = yes],[
    AC_DEFINE(HAVE_UUID_GENERATE_TIME_SAFE,[1],[Define if uuid_generate_time_safe is present in uuid/uuid.h.])],[
    AC_DEFINE(HAVE_UUID_GENERATE_TIME_SAFE,[0],[Define if uuid_generate_time_safe is present in uuid/uuid.h.])
  ])
])
