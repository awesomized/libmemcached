# ===========================================================================
#      http://www.gnu.org/software/autoconf-archive/ax_append_flag.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HARDEN_COMPILER_FLAGS
#
# DESCRIPTION
#
#   Any compiler flag that "hardens" or tests code.
#
#   NOTE: Implementation based on AX_APPEND_FLAG.
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

#serial 1

AC_DEFUN([AX_HARDEN_COMPILER_FLAGS],
[AC_REQUIRE([AX_CHECK_COMPILE_FLAG])
AC_REQUIRE([AX_APPEND_FLAG])
AC_REQUIRE([AX_CHECK_LINK_FLAG])
AX_APPEND_COMPILE_FLAGS([-O2])
AX_APPEND_COMPILE_FLAGS([-Werror])
AX_APPEND_COMPILE_FLAGS([-Wall])
AX_APPEND_COMPILE_FLAGS([-Wextra])
AX_APPEND_COMPILE_FLAGS([-Wmaybe-uninitialized])
AX_APPEND_COMPILE_FLAGS([-Wsign-compare])
AX_APPEND_COMPILE_FLAGS([-Wunused-result])
AX_APPEND_COMPILE_FLAGS([-Wunused-variable])
AX_APPEND_COMPILE_FLAGS([-floop-parallelize-all])
AX_APPEND_COMPILE_FLAGS([-fstack-protector-all])
AX_APPEND_COMPILE_FLAGS([-ggdb])
AX_APPEND_COMPILE_FLAGS([-fstack-protector-all])
AX_APPEND_COMPILE_FLAGS([-Wstack-protector])
AX_APPEND_COMPILE_FLAGS([-fwrapv])
AX_APPEND_COMPILE_FLAGS([-D_FORTIFY_SOURCE=2])
AX_APPEND_COMPILE_FLAGS([--param],[ssp-buffer-size=1])
AX_CHECK_LINK_FLAG([-Werror])
AX_CHECK_LINK_FLAG([-z relro -z now])
])dnl AX_HARDEN_COMPILER_FLAGS
