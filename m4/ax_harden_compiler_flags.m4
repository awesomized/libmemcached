# ===========================================================================
#      https://github.com/BrianAker/ddm4/
# ===========================================================================
#
# SYNOPSIS
#
#   AX_HARDEN_COMPILER_FLAGS()
#   AX_HARDEN_LINKER_FLAGS()
#   AX_HARDEN_CC_COMPILER_FLAGS()
#   AX_HARDEN_CXX_COMPILER_FLAGS()
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
# AX_APPEND_COMPILE_FLAGS_ERROR([-std=c++11]) -- Not ready yet
# AX_APPEND_COMPILE_FLAGS_ERROR([-pedantic]) -- ?
# AX_APPEND_COMPILE_FLAGS_ERROR([-Wstack-protector]) -- Issues on 32bit compile
# AX_APPEND_COMPILE_FLAGS_ERROR([-fstack-protector-all]) -- Issues on 32bit compile
# AX_APPEND_COMPILE_FLAGS_ERROR([-Wlong-long]) -- Don't turn on for compatibility issues memcached_stat_st

#serial 5

AC_DEFUN([AX_APPEND_LINK_FLAGS_ERROR],
    [AC_PREREQ([2.63])dnl
    AX_APPEND_LINK_FLAGS([$1],[LIB],[-Werror])
    ])

AC_DEFUN([AX_APPEND_COMPILE_FLAGS_ERROR],
    [AC_PREREQ([2.63])dnl
    AX_APPEND_COMPILE_FLAGS([$1],,[-Werror])
    ])

AC_DEFUN([AX_HARDEN_LINKER_FLAGS],
    [AC_PREREQ([2.63])dnl
    AC_REQUIRE([AX_APPEND_LINK_FLAGS])
    AC_REQUIRE([AX_CXX_COMPILER_VERSION])
    AC_REQUIRE([AX_DEBUG])
    AC_REQUIRE([AX_VCS_CHECKOUT])

    AX_APPEND_LINK_FLAGS_ERROR([-z relro -z now])
    AX_APPEND_LINK_FLAGS_ERROR([-pie])

    AS_IF([test "x$ac_cv_vcs_checkout" = xyes],[AX_APPEND_LINK_FLAGS_ERROR([-Werror])])
    ])

AC_DEFUN([AX_HARDEN_CC_COMPILER_FLAGS],
    [AC_PREREQ([2.63])dnl
    AC_REQUIRE([AX_APPEND_LINK_FLAGS])
    AC_REQUIRE([AX_CXX_COMPILER_VERSION])
    AC_REQUIRE([AX_DEBUG])
    AC_REQUIRE([AX_VCS_CHECKOUT])

    AC_LANG_PUSH([C])dnl

    AX_APPEND_COMPILE_FLAGS_ERROR([-g])
    AS_IF([test "x$ax_enable_debug" = xyes],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-ggdb])
      AX_APPEND_COMPILE_FLAGS_ERROR([-O0])],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-O2])])

    AX_APPEND_COMPILE_FLAGS_ERROR([-fstack-check])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-pragmas])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wall])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wextra])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunknown-pragmas])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wthis-test-should-fail])
    AX_APPEND_COMPILE_FLAGS_ERROR([-std=c99])
# Anything below this comment please keep sorted.
    AX_APPEND_COMPILE_FLAGS_ERROR([--param=ssp-buffer-size=1])
# -Wmissing-format-attribute
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-attributes])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Waddress])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Warray-bounds])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wbad-function-cast])
# Not in use -Wc++-compat
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wchar-subscripts])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wcomment])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wfloat-equal])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wformat-security])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wformat=2])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wlogical-op])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wmaybe-uninitialized])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wmissing-field-initializers])
    AS_IF([test "$MINGW" = "yes"],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-missing-noreturn])],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wmissing-noreturn])])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wmissing-prototypes])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wnested-externs])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wnormalized=id])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wold-style-definition])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Woverride-init])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wpointer-arith])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wpointer-sign])
    AS_IF([test "x$MINGW" = xyes],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=const])
        AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=noreturn])
        AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=pure])
        AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-redundant-decls])],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wredundant-decls])])

    AX_APPEND_COMPILE_FLAGS_ERROR([-Wshadow])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wshorten-64-to-32])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wsign-compare])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wstrict-overflow=1])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wstrict-prototypes])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wswitch-enum])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wundef])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunused])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunused-result])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunused-variable])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wwrite-strings])
    AX_APPEND_COMPILE_FLAGS_ERROR([-floop-parallelize-all])
    AX_APPEND_COMPILE_FLAGS_ERROR([-fwrapv])
    AX_APPEND_COMPILE_FLAGS_ERROR([-fmudflapt])
    AS_IF([test "x$ac_cv_vcs_checkout" = xyes], [AX_APPEND_COMPILE_FLAGS_ERROR([-Werror])])
    AC_LANG_POP([C])
  ])

AC_DEFUN([AX_HARDEN_CXX_COMPILER_FLAGS],
    [AC_PREREQ([2.63])dnl
    AC_REQUIRE([AX_APPEND_LINK_FLAGS])
    AC_REQUIRE([AX_CXX_COMPILER_VERSION])
    AC_REQUIRE([AX_DEBUG])
    AC_REQUIRE([AX_VCS_CHECKOUT])

    AC_LANG_PUSH([C++])

    AX_APPEND_COMPILE_FLAGS_ERROR([-g])
    AS_IF([test "x$ax_enable_debug" = xyes],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-O0])
      AX_APPEND_COMPILE_FLAGS_ERROR([-ggdb])],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-O2])])

    AS_IF([test "x$ac_c_gcc_recent" = xyes],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-D_FORTIFY_SOURCE=2])])

    AS_IF([test "x$ac_cv_vcs_checkout" = xyes],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-Werror])
      AX_APPEND_COMPILE_FLAGS_ERROR([-fstack-check])],
      [AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-pragmas])])

    AX_APPEND_COMPILE_FLAGS_ERROR([-Wall])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wextra])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunknown-pragmas])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wthis-test-should-fail])
# Anything below this comment please keep sorted.
    AX_APPEND_COMPILE_FLAGS_ERROR([--param=ssp-buffer-size=1])
# -Wmissing-format-attribute
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-attributes])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Waddress])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Warray-bounds])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wchar-subscripts])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wcomment])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wctor-dtor-privacy])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wfloat-equal])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wformat=2])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wmaybe-uninitialized])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wmissing-field-initializers])
    AS_IF([test "x$MINGW" = xyes],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-missing-noreturn])],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wmissing-noreturn])])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wlogical-op])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wnon-virtual-dtor])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wnormalized=id])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Woverloaded-virtual])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wpointer-arith])
    AS_IF([test "x$MINGW" = xyes],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=const])
        AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-suggest-attribute=noreturn])
        AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-error=suggest-attribute=noreturn])
        AX_APPEND_COMPILE_FLAGS_ERROR([-Wno-redundant-decls])],
        [AX_APPEND_COMPILE_FLAGS_ERROR([-Wredundant-decls])])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wshadow])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wshorten-64-to-32])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wsign-compare])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wstrict-overflow=1])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wswitch-enum])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wundef])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wc++11-compat])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunused])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunused-result])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wunused-variable])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wwrite-strings])
    AX_APPEND_COMPILE_FLAGS_ERROR([-Wformat-security])
    AX_APPEND_COMPILE_FLAGS_ERROR([-floop-parallelize-all])
    AX_APPEND_COMPILE_FLAGS_ERROR([-fwrapv])
    AX_APPEND_COMPILE_FLAGS_ERROR([-fmudflapt])
    AS_IF([test "x$ac_cv_vcs_checkout" = xyes], [AX_APPEND_COMPILE_FLAGS_ERROR([-Werror])])
    AC_LANG_POP([C++])
  ])

  AC_DEFUN([AX_HARDEN_COMPILER_FLAGS],
      [AC_PREREQ([2.63])dnl
      AC_REQUIRE([AX_APPEND_COMPILE_FLAGS])
      AC_REQUIRE([AX_APPEND_LINK_FLAGS])
      AC_REQUIRE([AX_CXX_COMPILER_VERSION])
      AC_REQUIRE([AX_DEBUG])
      AC_REQUIRE([AX_VCS_CHECKOUT])

      AC_CACHE_CHECK([if all warnings into errors],[ac_cv_warnings_as_errors],
        [AS_IF([test "x$ac_cv_vcs_checkout" = xyes],[ac_cv_warnings_as_errors=yes],
          [ac_cv_warnings_as_errors=no])
        ])

# All of the actual checks happen via these Macros
      AC_REQUIRE([AX_HARDEN_LINKER_FLAGS])
      AC_REQUIRE([AX_HARDEN_CC_COMPILER_FLAGS])
      AC_REQUIRE([AX_HARDEN_CXX_COMPILER_FLAGS])
      AC_REQUIRE([AX_CC_OTHER_FLAGS])

      AC_REQUIRE([gl_VISIBILITY])
      AS_IF([test -n "$CFLAG_VISIBILITY"],[CPPFLAGS="$CPPFLAGS $CFLAG_VISIBILITY"])
      ])

  AC_DEFUN([AX_CC_OTHER_FLAGS],
      [AC_PREREQ([2.63])dnl
      AC_REQUIRE([AX_APPEND_COMPILE_FLAGS_ERROR])
      AC_REQUIRE([AX_HARDEN_CC_COMPILER_FLAGS])

      AC_LANG_PUSH([C])
      AX_APPEND_COMPILE_FLAGS_ERROR([-pipe])
      AC_LANG_POP([C])
      ])
