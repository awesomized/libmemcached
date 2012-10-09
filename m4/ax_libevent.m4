# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIBEVENT()
#
# DESCRIPTION
#
#   libevent library
#
# LICENSE
#
#   Copyright (c) 2012 Brian Aker <brian@tangent.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1
 
  AC_DEFUN([AX_LIBEVENT],[
      AC_CACHE_CHECK([test for a working libevent], [ax_cv_libevent], [
        AX_SAVE_FLAGS
        LIBS="-levent $LIBS"
        AC_LANG_PUSH([C])
        AC_RUN_IFELSE([
          AC_LANG_PROGRAM([#include <event.h>], [
            struct event_base *tmp_event= event_init();
            event_base_free(tmp_event);
            ])],
          [ax_cv_libevent=yes],
          [ax_cv_libevent=no],
          [AC_MSG_WARN([test program execution failed])])
        AC_LANG_POP
        AX_RESTORE_FLAGS
        ])

      AS_IF([test "x$ax_cv_libevent" = "xyes"],[
        LIBEVENT_LDFLAGS="-levent"
        AC_SUBST(LIBEVENT_LDFLAGS)
        AC_DEFINE([HAVE_LIBEVENT],[1],[Define if event_init is present in event.h.])],[
        AC_DEFINE([HAVE_LIBEVENT],[0],[Define if event_init is present in event_init.h.])
        ])

      AM_CONDITIONAL(HAVE_LIBEVENT, test "x$ax_cv_libevent" = "xyes")
      ])

