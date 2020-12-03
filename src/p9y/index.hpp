#pragma once

#include "mem_config.h"

#if defined HAVE_STRINGS_H
# include <strings.h>
#elif defined __cplusplus
# include <cstring>
#else
# include <string.h>
#endif

#if !defined HAVE_INDEX
# define index strchr
#endif
