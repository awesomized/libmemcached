# ===========================================================================
#      https://github.com/BrianAker/ddm4/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C_COMPILER_VERSION()
#   AX_CXX_COMPILER_VERSION()
#
# DESCRIPTION
#
#   Capture version of C/C++ compiler
#
# LICENSE
#
#  Copyright (C) 2012 Brian Aker
#  All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#      * Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.
#  
#      * Redistributions in binary form must reproduce the above
#  copyright notice, this list of conditions and the following disclaimer
#  in the documentation and/or other materials provided with the
#  distribution.
#  
#      * The names of its contributors may not be used to endorse or
#  promote products derived from this software without specific prior
#  written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#serial 4

  AC_DEFUN([AX_C_COMPILER_VERSION],
      [AC_REQUIRE([AX_COMPILER_VENDOR])
      AC_MSG_CHECKING([C Compiler version])

      ax_c_compiler_version_vendor="$ax_c_compiler_vendor"

      AC_CHECK_DECL([__GNUC_PATCHLEVEL__],
        [GNUCC=yes
        ax_c_compiler_version_gcc="`$CC --dumpversion`"],
        [GNUCC=no])
      AC_MSG_CHECKING([GNUCC])
      AC_MSG_RESULT([$GNUCC])

      AC_CHECK_DECL([__SUNPRO_C],[SUNCC=yes],[SUNCC=no])
      AC_MSG_CHECKING([SUNCC])
      AC_MSG_RESULT([$SUNCC])

      AC_CHECK_DECL([__ICC],[INTELCC=yes],[INTELCC=no])
      AC_MSG_CHECKING([INTELCC])
      AC_MSG_RESULT([$INTELCC])

      AC_CHECK_DECL([__clang__],[CLANG=yes],[CLANG=no])
      AC_MSG_CHECKING([CLANG])
      AC_MSG_RESULT([$CLANG])

      AC_CHECK_DECL([__MINGW32__],
          [MINGW=yes
          ax_c_compiler_version_vendor=mingw],
          [MINGW=no])
      AC_MSG_CHECKING([MINGW])
      AC_MSG_RESULT([$MINGW])

      AS_CASE(["$ax_c_compiler_version_vendor"],
              [sun],[ax_c_compiler_version="`$CC -V 2>&1 | sed 1q`"],
              [intel],[ax_c_compiler_version="`$CC --version 2>&1 | sed 1q`"],
              [clang],[ax_c_compiler_version="`$CC --version 2>&1 | sed 1q`"],
              [gnu],[ax_c_compiler_version="`$CC --version | sed 1q`"],
              [mingw],[ax_c_compiler_version="`$CC --version | sed 1q`"],
              [ax_c_compiler_version=unknown])

      AC_MSG_RESULT(["$ax_c_compiler_version"])
      AC_SUBST([CC_VERSION_VENDOR],["$ax_c_compiler_version_vendor"])
      AC_SUBST([CC_VERSION],["$ax_c_compiler_version"])

      AS_IF([test "$GCC" = "yes"],
          [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if !defined(__GNUC__) || (__GNUC__ < 4) || ((__GNUC__ >= 4) && (__GNUC_MINOR__ < 2))
# error GCC is Too Old!
#endif
              ]])],
            [ac_c_gcc_recent=yes],
            [ac_c_gcc_recent=no])])
      ])

  AC_DEFUN([AX_CXX_COMPILER_VERSION],
      [AC_REQUIRE([AX_C_COMPILER_VERSION])
      AC_MSG_CHECKING([C++ Compiler version])

      AS_CASE(["$ax_c_compiler_version_vendor"],
        [sun],[ax_cxx_compiler_version="`$CC -V 2>&1 | sed 1q`"],
        [intel],[ax_cxx_compiler_version="`$CC --version 2>&1 | sed 1q`"],
        [clang],[ax_cxx_compiler_version="`$CC --version 2>&1 | sed 1q`"],
        [gnu],[ax_cxx_compiler_version="`$CC --version | sed 1q`"],
        [mingw],[ax_cxx_compiler_version="`$CC --version | sed 1q`"],
        [ax_cxx_compiler_version=unknown])

      AC_MSG_RESULT(["$ax_cxx_compiler_version"])
      AC_SUBST([CXX_VERSION_VENDOR],["$ax_c_compiler_version_vendor"])
      AC_SUBST([CXX_VERSION],["$ax_cxx_compiler_version"])
      ])
