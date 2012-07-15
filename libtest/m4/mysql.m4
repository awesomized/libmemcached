m4_include([libtest/m4/ax_lib_mysql.m4])
m4_include([libtest/m4/ax_prog_mysqld.m4])

AX_LIB_MYSQL([5.0])
AM_CONDITIONAL(HAVE_LIBMYSQL, test "x${found_mysql}" = "xyes")
AS_IF([test "x${found_mysql}" = "xyes"],
      [
       AC_DEFINE([HAVE_LIBMYSQL_BUILD], [1], [Define to 1 if MySQL libraries are available])
      ],
      [
       AC_DEFINE([HAVE_LIBMYSQL_BUILD], [0], [Define to 1 if MySQL libraries are available])
      ])
AX_PROG_MYSQLD
AS_IF([test -f "$ac_cv_path_MYSQLD"],
      [
        AC_DEFINE([HAVE_MYSQLD_BUILD], [1], [Define to 1 if MySQLD binary is available])
        AC_DEFINE_UNQUOTED([MYSQLD_BINARY], "$ac_cv_path_MYSQLD", [Name of the mysqld binary used in make test])
       ],
       [
        AC_DEFINE([HAVE_MYSQLD_BUILD], [0], [Define to 1 if MySQLD binary is available])
        AC_DEFINE([MYSQLD_BINARY], [0], [Name of the mysqld binary used in make test])
      ])

