#serial 2

AC_DEFUN([AX_DLOPEN],
    [AS_IF([test "x$enable_shared" = xyes],
      [AX_CHECK_LIBRARY([DL],[dlfcn.h],[dl],
        [AC_DEFINE([HAVE_LIBDL],[1],[Have dlopen])
        AC_CHECK_LIB([dl],[dlopen],[AC_DEFINE([HAVE_DLOPEN],[1],[Have dlopen])])
        AC_CHECK_LIB([dl],[dladdr],[AC_DEFINE([HAVE_DLADDR],[1],[Have dladdr])])])])
    ])

