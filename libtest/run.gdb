set logging on
set logging overwrite on
set environment LIBTEST_IN_GDB=1
run
thread apply all bt
quit
