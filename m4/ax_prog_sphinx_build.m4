# ===========================================================================
# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PROG_SPHINX_BUILD()
#
# DESCRIPTION
#
#   Look for sphinx-build
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

AC_DEFUN([AX_PROG_SPHINX_BUILD],
    [AC_PREREQ([2.63])dnl
    AC_CHECK_PROGS([PERL], [perl])
    AC_CHECK_PROGS([DPKG_GENSYMBOLS], [dpkg-gensymbols], [:])
    AC_CHECK_PROGS([LCOV], [lcov], [echo lcov not found])
    AC_CHECK_PROGS([LCOV_GENHTML], [genhtml], [echo genhtml not found])

    AC_CHECK_PROGS([SPHINXBUILD], [sphinx-build], [:])
    AS_IF([test "x${SPHINXBUILD}" != "x:"],[
      AC_CACHE_CHECK([if sphinx is new enough],[ac_cv_recent_sphinx],[

        ${SPHINXBUILD} -Q -C -b man -d conftest.d . . >/dev/null 2>&1
        AS_IF([test $? -eq 0],[ac_cv_recent_sphinx=yes],
          [ac_cv_recent_sphinx=no])
        rm -rf conftest.d
        ])
      ])

    AM_CONDITIONAL(HAVE_DPKG_GENSYMBOLS,[test "x${DPKG_GENSYMBOLS}" != "x:"])
    AM_CONDITIONAL(HAVE_SPHINX,[test "x${SPHINXBUILD}" != "x:"])
    AM_CONDITIONAL(HAVE_RECENT_SPHINX,[test "x${ac_cv_recent_sphinx}" = "xyes"])
])
