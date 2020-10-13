#include "mem_config.h"

#include <cstdlib>

#if HAVE_SETENV
# define SET_ENV_EX(n, k, v, overwrite) setenv(k, v, (overwrite))
#else // !HAVE_SETENV
# define SET_ENV_EX(n, k, v, overwrite) do { \
  static char n ## _env[] = k "=" v; \
  if ((overwrite) || !getenv(k)) { \
    putenv(n ## _env); \
  } \
} while(false)
#endif

#define SET_ENV(symbolic_name, literal_env_var, literal_env_val) \
  SET_ENV_EX(symbolic_name, literal_env_var, literal_env_val, true)
