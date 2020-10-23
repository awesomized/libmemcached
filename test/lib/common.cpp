#include "common.hpp"

#include <cstdlib>

const char *getenv_else(const char *var, const char *defval) {
  auto val = getenv(var);
  if (val && *val) {
    return val;
  }
  return defval;
}

