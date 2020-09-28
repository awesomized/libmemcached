#define CATCH_CONFIG_RUNNER
#include "lib/catch.hpp"
#include "mem_config.h"
#include <cstdlib>

#if HAVE_SETENV
# define SET_ENV(n, k, v) setenv(k, v, 0);
#else // !HAVE_SETENV
# define SET_ENV(n, k, v) static char n ## _env[] = k "=" v; putenv(n ## _env)
#endif

int main(int argc, char *argv[]) {

#if HAVE_ASAN
  SET_ENV(asan, "ASAN_OPTIONS", "halt_on_error=0")
#endif

#if LIBMEMCACHED_WITH_SASL_SUPPORT
  SET_ENV(sasl_pwdb, "MEMCACHED_SASL_PWDB", LIBMEMCACHED_WITH_SASL_PWDB);
  SET_ENV(sasl_conf, "SASL_CONF_PATH", LIBMEMCACHED_WITH_SASL_CONF);
#endif

  return Catch::Session().run(argc, argv);
}
