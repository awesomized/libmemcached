# ===========================================================================
#     http://tangent.org/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_CXX_CSTDINT
#
# DESCRIPTION
#
#  Example:
#
# LICENSE
#
#   Copyright (c) 2012 Brian Aker` <brian@tangent.org>
#
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU General Public License as published by the
#   Free Software Foundation, either version 3 of the License, or (at your
#   option) any later version.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#   Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program. If not, see <http://www.gnu.org/licenses/>.
#
#   As a special exception, the respective Autoconf Macro's copyright owner
#   gives unlimited permission to copy, distribute and modify the configure
#   scripts that are the output of Autoconf when processing the Macro. You
#   need not follow the terms of the GNU General Public License when using
#   or distributing such scripts, even though portions of the text of the
#   Macro appear in them. The GNU General Public License (GPL) does govern
#   all other use of the material that constitutes the Autoconf Macro.
#
#   This special exception to the GPL applies to versions of the Autoconf
#   Macro released by the Autoconf Archive. When you make and distribute a
#   modified version of the Autoconf Macro, you may extend this special
#   exception to the GPL to apply to your modified version as well.

#serial 1


AC_DEFUN([AX_CXX_CSTDINT],
    [
    AC_REQUIRE([AC_PROG_CXX])
    AC_REQUIRE([AC_PROG_CXXCPP])

    AC_MSG_CHECKING(the location of cstdint)
    AC_LANG_PUSH([C++])
    save_CXXFLAGS="${CXXFLAGS}"
    CXXFLAGS="${CXX_STANDARD} ${CXXFLAGS}"
    ac_cv_cxx_cstdint=""

    AC_LANG_PUSH([C++])
#    AC_CACHE_CHECK([for location of cstdint], [ac_cv_cxx_cstdint],
#      [
# Look for cstdint
      AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <cstdint>], [ uint32_t t ])],
        [ac_cv_cxx_cstdint="<cstdint>"],
        [
# Look for tr1/cstdint
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <tr1/cstdint>], [ uint32_t t ])],
          [ac_cv_cxx_cstdint="<tr1/cstdint>"],
          [
# Look for boost/cstdint.hpp
          AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <boost/cstdint.hpp>], [ uint32_t t ])],
            [ac_cv_cxx_cstdint="<boost/cstdint.hpp>"])

          ])
        ])
#      ])

  AC_LANG_POP()

  CXXFLAGS="${save_CXXFLAGS}"
  if test -n "$ac_cv_cxx_cstdint"; then
    AC_MSG_RESULT([$ac_cv_cxx_cstdint])
  else
    ac_cv_cxx_cstdint="<stdint.h>"
    AC_MSG_WARN([Could not find a cstdint header.])
    AC_MSG_RESULT([$ac_cv_cxx_cstdint])
  fi

  AC_DEFINE_UNQUOTED(CSTDINT_H,$ac_cv_cxx_cstdint, [the location of <cstdint>])

  ])
