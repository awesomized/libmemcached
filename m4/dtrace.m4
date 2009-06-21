dnl ---------------------------------------------------------------------------
dnl Macro: ENABLE_DTRACE
dnl ---------------------------------------------------------------------------
AC_DEFUN([ENABLE_DTRACE],[
  AC_ARG_ENABLE([dtrace],
    [AS_HELP_STRING([--enable-dtrace],
       [Build with support for the DTRACE. @<:@default=off@:>@])],
    [ac_cv_enable_dtrace="yes"],
    [ac_cv_enable_dtrace="no"])

  if test "$ac_cv_enable_dtrace" = "yes"
  then
    AC_PATH_PROG([DTRACE], [dtrace], "no", [/usr/sbin:$PATH])
    if test "x$DTRACE" != "xno"; then
      AC_DEFINE([HAVE_DTRACE], [1], [Enables DTRACE Support])
      DTRACE_HEADER=dtrace_probes.h

      # DTrace on MacOSX does not use -G option
      $DTRACE -G -o conftest.$$ -s libmemcached/libmemcached_probes.d 2>/dev/zero
      if test $? -eq 0
      then
        DTRACE_OBJ=libmemcached_probes.lo
        rm conftest.$$
      fi

      ac_cv_enable_dtrace="yes"
      AC_SUBST(HAVE_DTRACE)
    else
      AC_MSG_ERROR([Need dtrace binary and OS support.])
    fi
  fi

  AC_SUBST(DTRACEFLAGS)
  AC_SUBST(DTRACE_HEADER)
  AC_SUBST(DTRACE_OBJ)
  AM_CONDITIONAL([HAVE_DTRACE], [ test "$ac_cv_enable_dtrace" = "yes" ])
])
dnl ---------------------------------------------------------------------------
dnl End Macro: ENABLE_DTRACE
dnl ---------------------------------------------------------------------------
