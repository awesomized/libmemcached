# ===========================================================================
# https://github.com/BrianAker/ddm4
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HEX_VERSION
#
# DESCRIPTION
#
#   Generate version information in HEX format.
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
 
AC_DEFUN([AX_HEX_VERSION],[
    AC_CACHE_CHECK([generate HEX_VERSION], [ax_cv_hex_version],[
      ax_cv_hex_version=`echo $VERSION | sed 's|[\-a-z0-9]*$||' | awk -F. '{printf "0x%0.2d%0.3d%0.3d", $[]1, $[]2, $[]3}'`
      ])
    AC_SUBST([HEX_VERSION],["$ax_cv_hex_version"])
    AC_DEFINE([HEX_VERSION],[1],[HEX representation of VERSION])
    ])

