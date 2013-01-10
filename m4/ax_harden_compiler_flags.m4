# ===========================================================================
#      https://github.com/BrianAker/ddm4/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HARDEN_COMPILER_FLAGS() AX_HARDEN_LINKER_FLAGS()
#
# DESCRIPTION
#
#   Any compiler flag that "hardens" or tests code. C99 is assumed.
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

# The Following flags are not checked for
# -Wdeclaration-after-statement is counter to C99
# _APPEND_COMPILE_FLAGS_ERROR([-std=c++11]) -- Not ready yet
# _APPEND_COMPILE_FLAGS_ERROR([-pedantic]) --
# ?  _APPEND_COMPILE_FLAGS_ERROR([-Wlong-long]) -- Don't turn on for
# compatibility issues memcached_stat_st

#serial 7

AC_DEFUN([_WARNINGS_AS_ERRORS],
    [AC_CACHE_CHECK([if all warnings into errors],[ac_cv_warnings_as_errors],
      [AS_IF([test "x$ac_cv_vcs_checkout" = xyes],[ac_cv_warnings_as_errors=yes],
        [ac_cv_warnings_as_errors=no])
      ])
    ])

# Note: Should this be LIBS or LDFLAGS?
AC_DEFUN([_APPEND_LINK_FLAGS_ERROR],
         [AC_REQUIRE([AX_APPEND_LINK_FLAGS])
         AX_APPEND_LINK_FLAGS([$1],[LDFLAGS],[-Werror])
         ])

AC_DEFUN([_APPEND_COMPILE_FLAGS_ERROR],
         [AC_REQUIRE([AX_APPEND_COMPILE_FLAGS])
         AX_APPEND_COMPILE_FLAGS([$1],,[-Werror])
         ])

# Everything above this does the heavy lifting, while what follows does the specifics.

AC_DEFUN([_HARDEN_LINKER_FLAGS],
         [_APPEND_LINK_FLAGS_ERROR([-z relro -z now])
         #_APPEND_LINK_FLAGS_ERROR([-pie])
         AS_IF([test "x$ac_cv_warnings_as_errors" = xyes],
               [AX_APPEND_LINK_FLAGS([-Werror])])
         ])

AC_DEFUN([_HARDEN_CC_COMPILER_FLAGS],
         [AC_LANG_PUSH([C])dnl

         AS_IF([test "x$ax_enable_debug" = xyes],
           [CFLAGS=''
           _APPEND_COMPILE_FLAGS_ERROR([-ggdb])
           _APPEND_COMPILE_FLAGS_ERROR([-g])
           _APPEND_COMPILE_FLAGS_ERROR([-O0])],
           [_APPEND_COMPILE_FLAGS_ERROR([-g])
           _APPEND_COMPILE_FLAGS_ERROR([-O2])])

         AS_IF([test "x$ac_cv_vcs_checkout" = xyes],
           [_APPEND_COMPILE_FLAGS_ERROR([-fstack-check])],
           [_APPEND_COMPILE_FLAGS_ERROR([-Wno-pragmas])])

         _APPEND_COMPILE_FLAGS_ERROR([-Wall])
         _APPEND_COMPILE_FLAGS_ERROR([-Wextra])
         _APPEND_COMPILE_FLAGS_ERROR([-Wunknown-pragmas])
         _APPEND_COMPILE_FLAGS_ERROR([-Wthis-test-should-fail])
         _APPEND_COMPILE_FLAGS_ERROR([-std=c99])
# Anything below this comment please keep sorted.
         _APPEND_COMPILE_FLAGS_ERROR([--param=ssp-buffer-size=1])
# _APPEND_COMPILE_FLAGS_ERROR([-Wmissing-format-attribute])
          _APPEND_COMPILE_FLAGS_ERROR([-Wno-attributes])
          _APPEND_COMPILE_FLAGS_ERROR([-Waddress])
          _APPEND_COMPILE_FLAGS_ERROR([-Warray-bounds])
          _APPEND_COMPILE_FLAGS_ERROR([-Wbad-function-cast])
# Not in use -Wc++-compat
          _APPEND_COMPILE_FLAGS_ERROR([-Wchar-subscripts])
          _APPEND_COMPILE_FLAGS_ERROR([-Wcomment])
          _APPEND_COMPILE_FLAGS_ERROR([-Wfloat-equal])
          _APPEND_COMPILE_FLAGS_ERROR([-Wformat-security])
          _APPEND_COMPILE_FLAGS_ERROR([-Wformat=2])
          _APPEND_COMPILE_FLAGS_ERROR([-Wlogical-op])
          _APPEND_COMPILE_FLAGS_ERROR([-Wmaybe-uninitialized])
          _APPEND_COMPILE_FLAGS_ERROR([-Wmissing-field-initializers])
          AS_IF([test "x$MINGW" = xyes],
                [_APPEND_COMPILE_FLAGS_ERROR([-Wno-missing-noreturn])],
                [_APPEND_COMPILE_FLAGS_ERROR([-Wmissing-noreturn])])
          _APPEND_COMPILE_FLAGS_ERROR([-Wmissing-prototypes])
          _APPEND_COMPILE_FLAGS_ERROR([-Wnested-externs])
          _APPEND_COMPILE_FLAGS_ERROR([-Wnormalized=id])
          _APPEND_COMPILE_FLAGS_ERROR([-Wold-style-definition])
          _APPEND_COMPILE_FLAGS_ERROR([-Woverride-init])
          _APPEND_COMPILE_FLAGS_ERROR([-Wpointer-arith])
          _APPEND_COMPILE_FLAGS_ERROR([-Wpointer-sign])
          AS_IF([test "x$MINGW" = xyes],
                [_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=const])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=noreturn])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=pure])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-redundant-decls])],
                [_APPEND_COMPILE_FLAGS_ERROR([-Wredundant-decls])])

          _APPEND_COMPILE_FLAGS_ERROR([-Wshadow])
          _APPEND_COMPILE_FLAGS_ERROR([-Wshorten-64-to-32])
          _APPEND_COMPILE_FLAGS_ERROR([-Wsign-compare])
          _APPEND_COMPILE_FLAGS_ERROR([-Wstrict-overflow=1])
          _APPEND_COMPILE_FLAGS_ERROR([-Wstrict-prototypes])
          _APPEND_COMPILE_FLAGS_ERROR([-Wswitch-enum])
          _APPEND_COMPILE_FLAGS_ERROR([-Wundef])
          _APPEND_COMPILE_FLAGS_ERROR([-Wunused])
          _APPEND_COMPILE_FLAGS_ERROR([-Wunused-result])
          _APPEND_COMPILE_FLAGS_ERROR([-Wunused-variable])
          _APPEND_COMPILE_FLAGS_ERROR([-Wwrite-strings])
          _APPEND_COMPILE_FLAGS_ERROR([-floop-parallelize-all])
          _APPEND_COMPILE_FLAGS_ERROR([-fwrapv])
          _APPEND_COMPILE_FLAGS_ERROR([-fmudflapt])
          _APPEND_COMPILE_FLAGS_ERROR([-pipe])

          AS_IF([test "x$ax_enable_debug" = xno],
            [AS_IF([test "x$ac_cv_vcs_checkout" = xyes],
              [AS_IF([test "x${target_os}" != "xmingw"],
                [AS_IF([test "x$ac_c_gcc_recent" = xyes],
                  [_APPEND_COMPILE_FLAGS_ERROR([-D_FORTIFY_SOURCE=2])
                  #_APPEND_COMPILE_FLAGS_ERROR([-Wstack-protector])
                  #_APPEND_COMPILE_FLAGS_ERROR([-fstack-protector])
                  _APPEND_COMPILE_FLAGS_ERROR([-fstack-protector-all])
                  ])])])])

         AS_IF([test "x$ac_cv_warnings_as_errors" = xyes],
             [AX_APPEND_FLAG([-Werror])])

          AC_LANG_POP([C])
  ])

AC_DEFUN([_HARDEN_CXX_COMPILER_FLAGS],
         [AC_LANG_PUSH([C++])
         AC_REQUIRE([_APPEND_COMPILE_FLAGS_ERROR])

         AS_IF([test "x$ax_enable_debug" = xyes],
           [CXXFLAGS=''
           _APPEND_COMPILE_FLAGS_ERROR([-ggdb])
           _APPEND_COMPILE_FLAGS_ERROR([-g])
           _APPEND_COMPILE_FLAGS_ERROR([-O0])],
           [_APPEND_COMPILE_FLAGS_ERROR([-g])
           _APPEND_COMPILE_FLAGS_ERROR([-O2])])

         AS_IF([test "x$ac_cv_vcs_checkout" = xyes],
           [_APPEND_COMPILE_FLAGS_ERROR([-fstack-check])],
           [_APPEND_COMPILE_FLAGS_ERROR([-Wno-pragmas])])

         _APPEND_COMPILE_FLAGS_ERROR([-Wall])
         _APPEND_COMPILE_FLAGS_ERROR([-Wextra])
         _APPEND_COMPILE_FLAGS_ERROR([-Wunknown-pragmas])
         _APPEND_COMPILE_FLAGS_ERROR([-Wthis-test-should-fail])
# Anything below this comment please keep sorted.
         _APPEND_COMPILE_FLAGS_ERROR([--param=ssp-buffer-size=1])
# _APPEND_COMPILE_FLAGS_ERROR([-Wmissing-format-attribute])
          _APPEND_COMPILE_FLAGS_ERROR([-Wno-attributes])
          _APPEND_COMPILE_FLAGS_ERROR([-Waddress])
          _APPEND_COMPILE_FLAGS_ERROR([-Warray-bounds])
          _APPEND_COMPILE_FLAGS_ERROR([-Wchar-subscripts])
          _APPEND_COMPILE_FLAGS_ERROR([-Wcomment])
          _APPEND_COMPILE_FLAGS_ERROR([-Wctor-dtor-privacy])
          _APPEND_COMPILE_FLAGS_ERROR([-Wfloat-equal])
          _APPEND_COMPILE_FLAGS_ERROR([-Wformat=2])
          _APPEND_COMPILE_FLAGS_ERROR([-Wmaybe-uninitialized])
          _APPEND_COMPILE_FLAGS_ERROR([-Wmissing-field-initializers])
          _APPEND_COMPILE_FLAGS_ERROR([-Wlogical-op])
          _APPEND_COMPILE_FLAGS_ERROR([-Wnon-virtual-dtor])
          _APPEND_COMPILE_FLAGS_ERROR([-Wnormalized=id])
          _APPEND_COMPILE_FLAGS_ERROR([-Woverloaded-virtual])
          _APPEND_COMPILE_FLAGS_ERROR([-Wpointer-arith])
          AS_IF([test "x$MINGW" = xyes],
                [_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=const])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-missing-noreturn])
                _APPEND_COMPILE_FLAGS_ERROR([-Wmissing-noreturn])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=noreturn])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-error=suggest-attribute=noreturn])
                _APPEND_COMPILE_FLAGS_ERROR([-Wno-redundant-decls])],
                [_APPEND_COMPILE_FLAGS_ERROR([-Wredundant-decls])])
          _APPEND_COMPILE_FLAGS_ERROR([-Wshadow])
          _APPEND_COMPILE_FLAGS_ERROR([-Wshorten-64-to-32])
          _APPEND_COMPILE_FLAGS_ERROR([-Wsign-compare])
          _APPEND_COMPILE_FLAGS_ERROR([-Wstrict-overflow=1])
          _APPEND_COMPILE_FLAGS_ERROR([-Wswitch-enum])
          _APPEND_COMPILE_FLAGS_ERROR([-Wundef])
          _APPEND_COMPILE_FLAGS_ERROR([-Wc++11-compat])
          _APPEND_COMPILE_FLAGS_ERROR([-Wunused])
          _APPEND_COMPILE_FLAGS_ERROR([-Wunused-result])
          _APPEND_COMPILE_FLAGS_ERROR([-Wunused-variable])
          _APPEND_COMPILE_FLAGS_ERROR([-Wwrite-strings])
          _APPEND_COMPILE_FLAGS_ERROR([-Wformat-security])
          _APPEND_COMPILE_FLAGS_ERROR([-floop-parallelize-all])
          _APPEND_COMPILE_FLAGS_ERROR([-fwrapv])
          _APPEND_COMPILE_FLAGS_ERROR([-fmudflapt])
          _APPEND_COMPILE_FLAGS_ERROR([-pipe])

          AS_IF([test "x$ax_enable_debug" = xno],
          [AS_IF([test "x$ac_cv_vcs_checkout" = xyes],
            [AS_IF([test "x${target_os}" != "xmingw"],
              [AS_IF([test "x$ac_c_gcc_recent" = xyes],
                [_APPEND_COMPILE_FLAGS_ERROR([-D_FORTIFY_SOURCE=2])
                #_APPEND_COMPILE_FLAGS_ERROR([-Wstack-protector])
                #_APPEND_COMPILE_FLAGS_ERROR([-fstack-protector])
                _APPEND_COMPILE_FLAGS_ERROR([-fstack-protector-all])
                ])])])])

          AS_IF([test "x$ac_cv_warnings_as_errors" = xyes],
                [AX_APPEND_FLAG([-Werror])])
          AC_LANG_POP([C++])
  ])

# All of the heavy lifting happens in _HARDEN_LINKER_FLAGS,
# _HARDEN_CC_COMPILER_FLAGS, _HARDEN_CXX_COMPILER_FLAGS
  AC_DEFUN([AX_HARDEN_COMPILER_FLAGS],
           [AC_PREREQ([2.63])dnl
           AC_REQUIRE([_WARNINGS_AS_ERRORS])
           AC_REQUIRE([AX_APPEND_LINK_FLAGS])
           AC_REQUIRE([AX_COMPILER_VERSION])
           AC_REQUIRE([AX_DEBUG])
           AC_REQUIRE([AX_VCS_CHECKOUT])

           AC_REQUIRE([gl_VISIBILITY])
           AS_IF([test -n "$CFLAG_VISIBILITY"],[CPPFLAGS="$CPPFLAGS $CFLAG_VISIBILITY"])

           AC_REQUIRE([_HARDEN_LINKER_FLAGS])
           AC_REQUIRE([_HARDEN_CC_COMPILER_FLAGS])
           AC_REQUIRE([_HARDEN_CXX_COMPILER_FLAGS])
           ])
