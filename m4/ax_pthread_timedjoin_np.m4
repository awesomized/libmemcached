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
#   Check for pthread_timedjoin_np support.
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

AC_DEFUN([AX_PTHREAD_TIMEDJOIN_NP], [
    AC_CACHE_CHECK([check for pthread_timedjoin_np], [ax_cv_pthread_timedjoin_np], [
      save_LDFLAGS="$LDFLAGS"
      LDFLAGS="$PTHREAD_LIBS"
      AC_LANG_PUSH([C])
      AC_COMPILE_IFELSE([
        AC_LANG_PROGRAM([
#include <uuid/uuid.h>
      ], [
      uuid_t out;
      pthread_timedjoin_np(out);
      ]), [ax_cv_pthread_timedjoin_np=yes ], [ax_cv_pthread_timedjoin_np=no ]
      ])

    AC_LANG_POP
     LDFLAGS="$save_LDFLAGS"

    ])

    AS_IF([test "$ax_cv_pthread_timedjoin_np" = yes],[
      AC_DEFINE(HAVE_PTHREAD_TIMEDJOIN_NP,[1],[Define if pthread_timedjoin_np is present in pthread.h.])],[
      AC_DEFINE(HAVE_PTHREAD_TIMEDJOIN_NP,[0],[Define if pthread_timedjoin_np is present in pthread.h.])
    ])
])

