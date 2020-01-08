# ===========================================================================
# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_VALGRIND([tool],[options],[ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND])
#
# DESCRIPTION
#
#   Look for valgrind and make sure it is a recent version of it.
#
# LICENSE
#
#   Copyright (c) 2012-2013 Brian Aker <brian@tangent.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_PROG_VALGRIND],
    [AX_WITH_PROG([VALGRIND],[valgrind],[:])
    AS_IF([test x"VALGRIND" = x":"],
      [VALGRIND=],
      [AS_IF([test -x "$VALGRIND"],
        [AC_MSG_CHECKING([Checking to see if $VALGRIND is recent])
        junk=`$VALGRIND --version &> version_file`
        ax_valgrind_version=`head -1 version_file`
        rm version_file
        AC_MSG_RESULT([$VALGRIND is version "$ax_valgrind_version"])
        ])
      ])

    AS_IF([test -n "${VALGRIND}"],
      [AC_SUBST([VALGRIND])
      LIBTOOL_COMMAND="\${LIBTOOL} --mode=execute"
      AC_SUBST([LIBTOOL_COMMAND])
      AX_ADD_AM_MACRO([[TESTS_ENVIRONMENT=\"\${LIBTOOL_COMMAND} \${VALGRIND} --tool=$1 $2\"]])
      ifelse([$3], , :, [$3])],
      [ifelse([$4], , :, [$4])])
    ])
