dnl ---------------------------------------------------------------------------
dnl Macro: MEMCACHED_TEST
dnl ---------------------------------------------------------------------------
saved_CPPFLAGS="$CPPFLAGS"
CPPFLAGS=""
AC_ARG_WITH(memcached,
       [  --with-memcached=PATH   Specify path to memcached installation ],
       [
                if test "x$withval" != "xno" ; then
                        CPPFLAGS="-I${withval}/include"
                fi
       ]
)

AC_CHECK_HEADER(memcached/protocol_binary.h, [
                AC_RUN_IFELSE([ 
                   AC_LANG_PROGRAM([
                      #include "memcached/protocol_binary.h"
                   ], [
                      protocol_binary_request_set request;
                      if (sizeof(request) != sizeof(request.bytes)) {
                         return 1;
                      }
                   ])
                ],[ 
                   if test "x$CPPFLAGS" != "x" ; then
                      CPPFLAGS="$saved_CPPFLAGS $CPPFLAGS"
                   else
                      CPPFLAGS="$saved_CPPFLAGS"
                   fi
               ], AC_MSG_ERROR([Unsupported struct padding done by compiler.])) 
               ], [
                  AC_MSG_ERROR([Cannot locate memcached/protocol_binary.])
               ])

dnl ---------------------------------------------------------------------------
dnl End Macro: MEMCACHED_TEST
dnl ---------------------------------------------------------------------------
