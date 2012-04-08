# ===========================================================================
#  http://www.tangent.org/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SASL
#
# DESCRIPTION
#
#   Check for sasl version one or two support.
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

AC_DEFUN([AX_SASL_OPTION],[
    AC_REQUIRE([AX_SASL_CHECK])
    AC_ARG_ENABLE([sasl],
      [AS_HELP_STRING([--disable-sasl], [Build with sasl support @<:@default=on@:>@])],
      [ac_enable_sasl="$enableval"],
      [ac_enable_sasl="yes"])

    AS_IF([test "x$ac_enable_sasl" = "xyes"],
      [ac_enable_sasl="yes"],
      [ac_enable_sasl="no"])

    AS_IF([test "x$ac_enable_sasl" = "xyes"], [
      AC_DEFINE([HAVE_LIBSASL], [ 1 ], [Have libsasl2])
      ], [
      AC_DEFINE([HAVE_LIBSASL], [ 0 ], [Have libsasl2])
      ])

    AM_CONDITIONAL(HAVE_SASL, test "x${ac_enable_sasl}" = "xyes")

    ])

AC_DEFUN([AX_SASL_CHECK],[

    AX_CHECK_LIBRARY([LIBSASL], [sasl/sasl.h], [sasl2],[
      LIBSASL_LDFLAGS="-lsasl2"
      ax_cv_sasl=yes
      ], [
      ax_cv_sasl=no 
      ])

    AM_CONDITIONAL(HAVE_SASL, test "x${ac_enable_sasl}" = "xyes")
    ])
