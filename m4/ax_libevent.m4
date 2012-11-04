# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIBEVENT(), AX_LIBEVENT2(), AX_LIBEVENT2_EVHTTP()
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

#serial 2
 
AC_DEFUN([AX_LIBEVENT],
    [AC_PREREQ([2.63])dnl
    AC_CACHE_CHECK([test for a working libevent], [ax_cv_libevent], [
      AX_SAVE_FLAGS
      LIBS="-levent $LIBS"
      AC_LANG_PUSH([C])
      AC_RUN_IFELSE([
        AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <event.h>
          ], [
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
        AC_DEFINE([HAVE_LIBEVENT],[0],[Define if event_init is present in event.h.])
        ])

    AM_CONDITIONAL(HAVE_LIBEVENT, test "x$ax_cv_libevent" = "xyes")
    ])

  AC_DEFUN([AX_LIBEVENT2],[
      AC_REQUIRE([AX_LIBEVENT])
      AC_CACHE_CHECK([test for a working libevent version 2], [ax_cv_libevent2], [
        AX_SAVE_FLAGS
        LIBS="-levent $LIBS"
        AC_LANG_PUSH([C])
        AC_RUN_IFELSE([
          AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <event2/event.h>
            ],[
            struct event_base *tmp_event= event_init();
            event_base_free(tmp_event);
            ])],
          [ax_cv_libevent2=yes],
          [ax_cv_libevent2=no],
          [AC_MSG_WARN([test program execution failed])])
        AC_LANG_POP
        AX_RESTORE_FLAGS
        ])

      AS_IF([test "x$ax_cv_libevent2" = "xyes"],[
        LIBEVENT2_LDFLAGS="-levent"
        AC_SUBST(LIBEVENT2_LDFLAGS)
        AC_DEFINE([HAVE_LIBEVENT2],[1],[Define if event_init is present in event2/event.h.])],[
        AC_DEFINE([HAVE_LIBEVENT2],[0],[Define if event_init is present in event2/event.h.])
        ])

      AM_CONDITIONAL(HAVE_LIBEVENT2, test "x$ax_cv_libevent2" = "xyes")
      ])

  AC_DEFUN([AX_LIBEVENT2_EVHTTP],[
      AC_REQUIRE([AX_LIBEVENT2])
      AC_CACHE_CHECK([test for a working libevent2 evhttp interface], [ax_cv_libevent2_evhttp], [
        AX_SAVE_FLAGS
        LIBS="-levent $LIBS"
        AC_LANG_PUSH([C])
        AC_RUN_IFELSE([
          AC_LANG_PROGRAM([
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <event2/event.h>
#include <event2/http.h>
            ],[
            struct event_base *libbase= event_base_new();  
            struct evhttp *libsrvr= evhttp_new(libbase);
            ])],
          [ax_cv_libevent2_evhttp=yes],
          [ax_cv_libevent2_evhttp=no],
          [AC_MSG_WARN([test program execution failed])])
        AC_LANG_POP
        AX_RESTORE_FLAGS
        ])

      AS_IF([test "x$ax_cv_libevent2_evhttp" = "xyes"],[
        LIBEVENT2_LDFLAGS="-levent"
        AC_SUBST(LIBEVENT2_LDFLAGS)
        AC_DEFINE([HAVE_LIBEVENT2],[1],[Define if event_init is present in event2/event.h.])],[
        AC_DEFINE([HAVE_LIBEVENT2],[0],[Define if event_init is present in event2/event.h.])
        ])

      AM_CONDITIONAL(HAVE_LIBEVENT2_EVHTTP, test "x$ax_cv_libevent2_evhttp" = "xyes")
      ])
