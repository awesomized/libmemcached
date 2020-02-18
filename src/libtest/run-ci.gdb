set logging on
set logging overwrite on
set environment LIBTEST_IN_GDB=1
set ASAN_OPTIONS=abort_on_error=1
run
thread apply all bt
quit
