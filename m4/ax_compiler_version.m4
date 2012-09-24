# ===========================================================================
#      https://github.com/BrianAker/ddm4/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_C_COMPILER_VERSION
#   AX_CXX_COMPILER_VERSION
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

#serial 3

  AC_DEFUN([AX_C_COMPILER_VERSION],[
      AC_REQUIRE([AX_COMPILER_VENDOR])
      AC_MSG_CHECKING("C Compiler version")

      AC_CHECK_DECL([__GNUC_PATCHLEVEL__],[GNUCC="yes"], [GNUCC="no"])
      AC_CHECK_DECL([__SUNPRO_C], [SUNCC="yes"],[SUNCC="no"])
      AC_CHECK_DECL([__ICC], [INTELCC="yes"],[INTELCC="no"])
      AC_CHECK_DECL([__clang__], [CLANG="yes"],[CLANG="no"])

      ax_cc_version=unknown
      AS_IF([test "$ax_cc_version" = "unknown"],[AS_IF([test "$SUNCC" = "yes"],[ax_cc_version=`$CC -V 2>&1 | sed 1q`])])
      AS_IF([test "$ax_cc_version" = "unknown"],[AS_IF([test "$CLANG" = "yes"],[ax_cc_version=`$CC --version 2>&1 | sed 1q` ])])
      AS_IF([test "$ax_cc_version" = "unknown"],[AS_IF([test "$INTELCC" = "yes"],[ax_cc_version=`$CC --version 2>&1 | sed 1q` ])])
      AS_IF([test "$ax_cc_version" = "unknown"],[AS_IF([test "$GNUCC" = "yes"],[ax_cc_version=`$CC --version | sed 1q` ])])

      AC_MSG_RESULT(["$ax_cc_version"])
      AC_SUBST([CC_VERSION],["$ax_cc_version"])
      ])

  AC_DEFUN([AX_CXX_COMPILER_VERSION], [
      AC_REQUIRE([AX_C_COMPILER_VERSION])
      AC_MSG_CHECKING("C++ Compiler version")

      ax_cxx_version=unknown
      AS_IF([test "$ax_cxx_version" = "unknown"],[AS_IF([test "$GNUCC" = "yes"],[ax_cxx_version=`$CXX --version | sed 1q`])])
      AS_IF([test "$ax_cxx_version" = "unknown"],[AS_IF([test "$SUNCC" = "yes"],[ax_cxx_version=`$CXX -V 2>&1 | sed 1q`])])
      AS_IF([test "$ax_cxx_version" = "unknown"],[AS_IF([test "$CLANG" = "yes"],[ax_cxx_version=`$CXX --version 2>&1 | sed 1q`])])
      AS_IF([test "$ax_cxx_version" = "unknown"],[AS_IF([test "$INTELCC" = "yes"],[ax_cc_version=`$CCX --version 2>&1 | sed 1q` ])])

      AC_MSG_RESULT(["$ax_cxx_version"])
      AC_SUBST([CXX_VERSION], ["$ax_cxx_version"])
      ])
