#pragma once

#include "mem_config.h"

#if defined __cplusplus
#   include <cstdlib>
#else
#   include <stdlib.h>
#endif

#if defined HAVE_LIBGEN_H
# include <libgen.h>
#elif defined _WIN32
# if defined __cplusplus
extern "C" {
# endif
# define P9Y_NEED_BASENAME
char *basename(const char *filename);
# if  defined __cplusplus
}
# endif
#endif // HAVE_LIBGEN_H
