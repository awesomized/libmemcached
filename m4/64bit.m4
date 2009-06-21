dnl ---------------------------------------------------------------------------
dnl Macro: ENABLE_64BIT
dnl ---------------------------------------------------------------------------
AC_DEFUN([ENABLE_64BIT],[

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
         CFLAGS="-m64 $CFLAGS"
         CXXFLAGS="-m64 $CXXFLAGS"
         if test "$target_cpu" = "sparc" -a "x$SUNCC" = "xyes"
         then
            CFLAGS="-xmemalign=8s $CFLAGS"
            CXXFLAGS="-xmemalign=8s $CXXFLAGS"
         fi
       ])
])
dnl ---------------------------------------------------------------------------
dnl End Macro: ENABLE_64BIT
dnl ---------------------------------------------------------------------------
