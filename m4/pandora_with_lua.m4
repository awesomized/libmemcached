dnl -*- mode: m4; c-basic-offset: 2; indent-tabs-mode: nil; -*-
dnl vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
dnl   
dnl pandora-build: A pedantic build system
dnl Copyright (C) 2009 Sun Microsystems, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.
dnl
dnl From Monty Taylor

AC_DEFUN([PANDORA_WITH_LUA],[
    dnl Check for lua
    AC_ARG_WITH([lua], 
    [AS_HELP_STRING([--with-lua],
      [Build Lua Bindings @<:@default=yes@:>@])],
    [with_lua=$withval], 
    [with_lua=yes])

  AS_IF([test "x$with_lua" != "xno"],[
    AS_IF([test "x$with_lua" = "xyes"],
      [LUAPC=lua],
      [LUAPC=$with_lua])

    PKG_CHECK_MODULES([LUA], $LUAPC >= 5.1, [
      AC_DEFINE([HAVE_LUA], [1], [liblua])
      AC_DEFINE([HAVE_LUA_H], [1], [lua.h])
      with_lua=yes
    ],[
      PKG_CHECK_MODULES([LUA], lua5.1 >= 5.1, [
        AC_DEFINE([HAVE_LUA], [1], [liblua])
        AC_DEFINE([HAVE_LUA_H], [1], [lua.h])
	with_lua=yes
      ],[
        AC_DEFINE([HAVE_LUA],["x"],["x"])
        with_lua=no
      ])
    ])

   AC_SUBST(LUA_CFLAGS)
   AC_SUBST(LUA_LIBS)
  ])
  AM_CONDITIONAL(BUILD_LUA, test "$with_lua" = "yes")

])
