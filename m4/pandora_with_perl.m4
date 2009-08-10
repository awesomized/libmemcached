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


AC_DEFUN([PANDORA_WITH_PERL], [

  AC_ARG_WITH([perl], 
    [AS_HELP_STRING([--with-perl],
      [Build Perl Bindings @<:@default=yes@:>@])],
    [with_perl=$withval], 
    [with_perl=yes])


  AS_IF([test "x$with_perl" != "xno"],[
    AS_IF([test "x$with_perl" != "xyes"],
      [ac_chk_perl=$with_perl],
      [ac_chk_perl=perl])
    AC_CHECK_PROGS(PERL,$ac_chk_perl)
  ])

  AM_CONDITIONAL(BUILD_PERL, [test "$with_perl" = "yes"])

dnl Don't think we need these anymore, but it's a good reference
dnl if test "x$PERL" != "x"; then
dnl  PERLCCFLAGS=`$PERL -MConfig -e 'print $Config{ccflags};'`
dnl  PERLCPPFLAGS=`$PERL -MConfig -e 'print $Config{cppflags};'`
dnl  PERLLIBS=`$PERL -MConfig -e 'print $Config{perllibs};'`
dnl fi 

])
