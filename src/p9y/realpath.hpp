#pragma once

#include "mem_config.h"

#if defined __cplusplus
# include <cstdlib>
#else
# include <stdlib.h>
#endif

#if !defined HAVE_REALPATH
# define P9Y_NEED_REALPATH
# if defined __cplusplus
extern "C" {
# endif
char *realpath(const char *path, char real[_MAX_PATH]);
# if defined __cplusplus
}
# endif
#endif // HAVE_REALPATH
