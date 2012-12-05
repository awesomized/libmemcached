# ===========================================================================
#  http://www.tangent.org/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_SASL
#   AX_SASL_CHECK
#
# DESCRIPTION
#
#   Check to see if libsasl is requested and available.
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

AC_DEFUN([AX_SASL_OPTION],
    [AC_ARG_ENABLE([sasl],
      [AS_HELP_STRING([--disable-sasl], [Build with sasl support @<:@default=on@:>@])],
      [ac_enable_sasl="$enableval"],
      [ac_enable_sasl=yes])

    ax_sasl_option=no
    AS_IF([test "x${ac_enable_sasl}" = xyes],
      [AX_SASL_CHECK
      echo "$ax_sasl_check"
      AS_IF([test "x${ax_sasl_check}" = xyes],
        [ax_sasl_option=yes],
        [AC_MSG_WARN([request to add sasl support failed, please see config.log])
        ac_enable_sasl=no])
      ])

    AC_MSG_CHECKING([checking to see if enabling sasl])
    AC_MSG_RESULT(["$ax_sasl_option"])
    AM_CONDITIONAL([HAVE_SASL],[test "x${ax_sasl_option}" = xyes])
    ])

AC_DEFUN([AX_SASL_CHECK],
    [ax_sasl_check=no
    AX_CHECK_LIBRARY([LIBSASL],[sasl/sasl.h],[sasl2],
      [ax_sasl_check=yes
      AC_SUBST([SASL_LIB],[[-lsasl2]])
      ])
    AC_MSG_CHECKING([checking to see if sasl works])
    AC_MSG_RESULT(["$ax_sasl_check"])
    ])
