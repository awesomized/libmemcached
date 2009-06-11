dnl ---------------------------------------------------------------------------
dnl Macro: deprecated
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE([deprecated],
  [AS_HELP_STRING([--enable-deprecated],
     [Enable deprecated interface @<:@default=off@:>@])],
  [ac_enable_deprecated="$enableval"],
  [ac_enable_deprecated="no"])

if test "x$ac_enable_deprecated" = "xyes"
then
   DEPRECATED="#define MEMCACHED_ENABLE_DEPRECATED 1"
fi

AC_SUBST(DEPRECATED)
dnl ---------------------------------------------------------------------------
dnl End Macro: deprecated
dnl ---------------------------------------------------------------------------
