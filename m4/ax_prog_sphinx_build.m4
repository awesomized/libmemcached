# ===========================================================================
# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_SPHINX_BUILD([ACTION-IF-FOUND], [ACTION-IF-NOT_FOUND])
#
# DESCRIPTION
#
#   Look for sphinx-build and make sure it is a recent version of it.
#
# LICENSE
#
#   Copyright (c) 2012-2013 Brian Aker <brian@tangent.org>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_PROG_SPHINX_BUILD],
    [AC_PREREQ([2.63])dnl
    AC_CHECK_PROGS([SPHINXBUILD], [sphinx-build], [:])
    AS_IF([test "x${SPHINXBUILD}" != "x:"],[
      AC_CACHE_CHECK([if sphinx is new enough],[ac_cv_recent_sphinx],[

        ${SPHINXBUILD} -Q -C -b man -d conftest.d . . >/dev/null 2>&1
        AS_IF([test $? -eq 0],[ac_cv_recent_sphinx=yes],
          [ac_cv_recent_sphinx=no])
        rm -rf conftest.d
        ])
      ])

    AM_CONDITIONAL([HAVE_SPHINX],[test "x${SPHINXBUILD}" != "x:"])
    AM_CONDITIONAL([HAVE_RECENT_SPHINX],[test "x${ac_cv_recent_sphinx}" = "xyes"])

    AS_IF([test "x${ac_cv_recent_sphinx}" = "xyes"],
        [ifelse([$2], , :, [$2])],
        [ifelse([$3], , :, [$3])])
])
