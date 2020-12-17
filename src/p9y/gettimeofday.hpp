#pragma once

#include "libmemcached-1/platform.h"

#include <time.h>
#if defined HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#if !defined HAVE_GETTIMEOFDAY
int gettimeofday(struct timeval* tp, struct timezone* tzp);
#endif
