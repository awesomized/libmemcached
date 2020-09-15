#define CATCH_CONFIG_RUNNER
#include "lib/catch.hpp"
#include <cstdlib>

int main(int argc, char *argv[]) {
#if HAVE_ASAN
# if HAVE_SETENV
  setenv("ASAN_OPTIONS", "halt_on_error=0", 0);
# else
  char env[] = "ASAN_OPTIONS=halt_on_error=0";
  putenv(env);
# endif
#endif
  return Catch::Session().run(argc, argv);
}
