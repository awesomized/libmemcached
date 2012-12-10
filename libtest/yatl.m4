dnl Copyright (C) 2012 Data Differential, LLC.
dnl This file is free software; Data Differential, LLC.
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_SUBST([LIBTEST_VERSION],[1.0])
AC_CONFIG_FILES([libtest/version.h])

m4_include([libtest/m4/mysql.m4])
m4_include([libtest/m4/memcached.m4])
AC_DEFINE([HAVE_MEMCACHED_SASL_BINARY],[0],[If Memcached binary with sasl support is available])

YATL_MYSQL
YATL_MEMCACHED

AC_CONFIG_FILES([libtest/yatlcon.h])
