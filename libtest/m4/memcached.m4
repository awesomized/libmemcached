#serial 1

  AC_DEFUN([YATL_MEMCACHED], [
      AC_REQUIRE([AX_ENABLE_LIBMEMCACHED])

      AX_WITH_PROG(MEMCACHED_BINARY, [memcached])
      AS_IF([test -f "$ac_cv_path_MEMCACHED_BINARY"],[
        AC_DEFINE([HAVE_MEMCACHED_BINARY], [1], [If Memcached binary is available])
        AC_DEFINE_UNQUOTED([MEMCACHED_BINARY], "$ac_cv_path_MEMCACHED_BINARY", [Name of the memcached binary used in make test])
        ],[
        AC_DEFINE([HAVE_MEMCACHED_BINARY], [1], [If Memcached binary is available])
        AC_DEFINE([MEMCACHED_BINARY], ["memcached/memcached"], [Name of the memcached binary used in make test])
        ])
      ])
