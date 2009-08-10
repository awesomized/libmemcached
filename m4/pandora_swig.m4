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

AC_DEFUN([PANDORA_SWIG],[

  AC_PROG_SWIG(1.3.36)
  AC_DEFINE([SWIG_TYPE_TABLE],
    [libgearman],
    [Type Table name for SWIG symbol table])

  dnl Have to hard-code /usr/local/include and /usr/include into the path.
  dnl I hate this. Why is swig sucking me
  SWIG="$SWIG \${DEFS} -I\${top_srcdir} -I\${top_builddir} -I/usr/local/include -I/usr/include"
  AC_SUBST([SWIG])


  AC_CACHE_CHECK([if swig supports Python3],
    [ac_cv_swig_has_python3_],
    [
      AS_IF([swig -python3 2>&1 | grep "Unable to find" > /dev/null],
        [ac_cv_swig_has_python3_=no],
        [ac_cv_swig_has_python3_=yes])
    ])
])
