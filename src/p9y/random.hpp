#pragma once

#include "mem_config.h"

#if defined __cplusplus
# include <cstdlib>
#else
# include <stdlib.h>
#endif

#if !defined HAVE_RANDOM
# if defined HAVE_RAND
#   define random rand
#   define srandom srand
# endif
#endif
