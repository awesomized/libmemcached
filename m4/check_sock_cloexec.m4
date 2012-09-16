# Author?
# AX_CHECK_SOCK_CLOEXEC([action-if-found], [action-if-not-found])
#

#serial 1

AC_DEFUN([AX_CHECK_SOCK_CLOEXEC], [
    AC_CACHE_CHECK([whether SOCK_CLOEXEC is supported], [ax_cv_sock_cloexec], [
      AC_LANG_PUSH([C])
      AC_RUN_IFELSE([
        AC_LANG_PROGRAM([

          /* SOCK_CLOEXEC test */
#include <sys/types.h>
#include <sys/socket.h>

          ], [
          int s= socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
          return (s == -1);
          ])],
        [ax_cv_sock_cloexec=yes],
        [ax_cv_sock_cloexec=no])
      ])

    AS_IF([test x"ax_cv_sock_cloexec" = xyes], [AC_MSG_RESULT([yes]) ; $1], AC_MSG_RESULT([no]) ; $2)
    ])
