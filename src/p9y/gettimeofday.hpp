#pragma once

#include "libmemcached-1.0/platform.h"

#if defined __cplusplus
# include <ctime>
#else
# include <time.h>
#endif

#if defined HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#if !defined HAVE_GETTIMEOFDAY
# define P9Y_NEED_GETTIMEOFDAY
#endif

#if defined P9Y_NEED_GETTIMEOFDAY
# if defined __cplusplus
extern "C" {
# endif
int gettimeofday(struct timeval* tp, struct timezone* tzp);
# if defined __cplusplus
};
#endif
#endif
