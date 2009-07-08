dnl  Copyright (C) 2009 Sun Microsystems
dnl This file is free software; Sun Microsystems
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl ---------------------------------------------------------------------------
dnl Macro: PANDORA_64BIT
dnl ---------------------------------------------------------------------------
AC_DEFUN([PANDORA_64BIT],[

  AC_CHECK_PROGS(ISAINFO, [isainfo], [no])
  AS_IF([test "x$ISAINFO" != "xno"],
        [isainfo_b=`${ISAINFO} -b`],
        [isainfo_b="x"])

  AS_IF([test "$isainfo_b" != "x"],
        [AC_ARG_ENABLE([64bit],
             [AS_HELP_STRING([--disable-64bit],
                [Build 64 bit binary @<:@default=on@:>@])],
             [ac_enable_64bit="$enableval"],
             [ac_enable_64bit="yes"])])

  AS_IF([test "x$ac_enable_64bit" = "xyes"],[
         if test "x$libdir" = "x\${exec_prefix}/lib" ; then
           # The user hasn't overridden the default libdir, so we'll 
           # the dir suffix to match solaris 32/64-bit policy
           isainfo_k=`${ISAINFO} -k` 
           libdir="${libdir}/${isainfo_k}"
         fi
         CPPFLAGS="-m64 ${CPPFLAGS}"
         LDFLAGS="-m64 ${LDFLAGS}"
         DTRACEFLAGS="${DTRACEFLAGS} -64"
         if test "$target_cpu" = "sparc" -a "x$SUNCC" = "xyes"
         then
            AM_CFLAGS="-xmemalign=8s ${AM_CFLAGS}"
            AM_CXXFLAGS="-xmemalign=8s ${AM_CXXFLAGS}"
         fi
       ],[DTRACEFLAGS="${DTRACEFLAGS} -32"])
])
dnl ---------------------------------------------------------------------------
dnl End Macro: PANDORA_64BIT
dnl ---------------------------------------------------------------------------
