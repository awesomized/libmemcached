#pragma once

#include "mem_config.h"

#if defined __cplusplus
# include <cstdlib>
#else
# include <stdlib.h>
#endif

#if !defined HAVE_REALPATH
# if defined _WIN32
#  if defined __cplusplus
extern "C" {
#  endif
char *realpath(const char *path, char real[_MAX_PATH]);
#  if defined __cplusplus
}
#  endif
# endif // _WIN32
#endif // HAVE_REALPATH
