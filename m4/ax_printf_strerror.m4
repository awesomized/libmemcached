#   ===========================================================================
#   https://github.com/BrianAker/ddm4/
#   ===========================================================================
#
# SYNOPSIS
#
#   AX_PRINTF_STRERROR()
#
# DESCRIPTION
#
#   Check to see if printf() has %m implemented (i.e. can handle strerror
#   internally)
#
# LICENSE
#
#  Copyright (C) 2013 Brian Aker All rights reserved.
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

#serial 2

AC_DEFUN([AX_PRINTF_STRERROR],
         [AC_PREREQ([2.63])dnl
         AC_CACHE_CHECK([printf() has strerror option],
         [ax_cv_have_printf_strerror],
         [AC_LANG_PUSH([C++])
         AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <cstdio>
                        #include <cstddef>
                        #include <cstdlib>
                        #include <cstring>
                        #include <cerrno>
                        ]],
                        [[char buffer[1024]; errno=0; snprintf(buffer, sizeof(buffer), "%m"); if (strcmp("Success", buffer) !=0 ) return EXIT_FAILURE;]])],
                        [ax_cv_have_printf_strerror=yes],
                        [ax_cv_have_printf_strerror=no],
                        [ax_cv_have_printf_strerror=no])
         AC_LANG_POP])
         AC_MSG_RESULT(["$ax_cv_have_printf_strerror"])
         AS_IF([test "x$ax_cv_have_printf_strerror" = xyes],
               [AC_DEFINE([HAVE_PRINTF_STRERROR],[1],[define if printf supports directly print errno])])
         ])
