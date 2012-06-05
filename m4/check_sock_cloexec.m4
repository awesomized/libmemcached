dnl CHECK_SOCK_CLOEXEC([action-if-found], [action-if-not-found])
AC_DEFUN([AX_CHECK_SOCK_CLOEXEC], [{
    AC_MSG_CHECKING(whether SOCK_CLOEXEC is supported)
    AC_TRY_RUN([/* SOCK_CLOEXEC test */
#include <sys/types.h>
#include <sys/socket.h>

int main (int argc, char *argv [])
{
    int s= socket(PF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    return (s == -1);
}
    ],
    [AC_MSG_RESULT(yes) ; ax_cv_sock_cloexec="yes" ; $1],
    [AC_MSG_RESULT(no)  ; ax_cv_sock_cloexec="no"  ; $2],
    [AC_MSG_RESULT(not during cross-compile) ; ax_cv_sock_cloexec="no"]
    )
}])
