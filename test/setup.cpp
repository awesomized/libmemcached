#include "mem_config.h"
#include "test/conf.h"
#include "test/lib/env.hpp"
#include "test/lib/random.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <iostream>
#include <string>
#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

static void sigchld(int, siginfo_t *si, void *) {
  switch (si->si_code) {
  case CLD_EXITED:
    // expected
    break;
  case CLD_KILLED:
    if (si->si_status == SIGKILL) {
      // expected
      break;
    }
    [[fallthrough]];
  case CLD_DUMPED:
    std::cerr << "Server{pid="
              << std::to_string(si->si_pid)
              << "} died: "
              << strsignal(si->si_status)
              << std::endl;
    break;
  }
}

static inline void setup_signals() {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_flags = SA_NOCLDSTOP | SA_RESTART | SA_SIGINFO | SA_NODEFER;

  sa.sa_sigaction = sigchld;
  if (0 > sigaction(SIGCHLD, &sa, nullptr)) {
    perror("sigaction(CHLD)");
  }
}

#if HAVE_ASAN
// this has to be a macro for putenv() to work
# define ASAN_OPTIONS \
    "check_initialization_order=1," \
    "replace_str=1," \
    "replace_intrin=1," \
    "detect_stack_use_after_return=1," \
    "alloc_dealloc_mismatch=1," \
    "new_delete_type_mismatch=1," \
    "detect_odr_violation=2," \
    "halt_on_error=0," \
    "verify_asan_link_order=1," \
    "abort_on_error=0," \
    ""
# define LSAN_OPTIONS \
  "suppressions=" SOURCES_ROOT "/test/LeakSanitizer.suppressions," \
  ""

static inline void setup_asan(char **argv) {
  const auto set = getenv("ASAN_OPTIONS");

  if (!set || !*set) {
    SET_ENV_EX(asan, "ASAN_OPTIONS", ASAN_OPTIONS, 0);
    execvp(argv[0], argv);
    perror("exec()");
  }
}
static inline void setup_lsan(char **argv) {
  const auto set = getenv("LSAN_OPTIONS");

  if (!set || !*set) {
    SET_ENV_EX(lsan, "LSAN_OPTIONS", LSAN_OPTIONS, 0);
    execvp(argv[0], argv);
    perror("exec()");
  }
}
#else
# define setup_asan(a) (void) a
# define setup_lsan(a) (void) a
#endif

#if HAVE_TSAN
# define TSAN_OPTIONS \
    "abort_on_error=0," \
    "halt_on_error=0" \
    ""
static inline void setup_tsan(char **argv) {
  const auto set = getenv("TSAN_OPTIONS");

  if (!set || !*set) {
    SET_ENV_EX(tsan, "TSAN_OPTIONS", TSAN_OPTIONS, 0);
    execvp(argv[0], argv);
    perror("exec()");
  }
}
#else
# define setup_tsan(a) (void) a
#endif

#if LIBMEMCACHED_WITH_SASL_SUPPORT
static inline void setup_sasl() {
  SET_ENV_EX(sasl_pwdb, "MEMCACHED_SASL_PWDB", LIBMEMCACHED_WITH_SASL_PWDB, 0);
  SET_ENV_EX(sasl_conf, "SASL_CONF_PATH", LIBMEMCACHED_WITH_SASL_CONF, 0);
}
#else
# define setup_sasl()
#endif

static inline void setup_random() {
  random_setup();
}

int setup(int &, char ***argv) {
  setup_signals();
  setup_random();
  setup_asan(*argv);
  setup_lsan(*argv);
  setup_tsan(*argv);
  setup_sasl();

  return 0;
}
