#pragma once

#include "mem_config.h"

#if defined __cplusplus
# include <ctime>
#else
# include <time.h>
#endif

#ifndef HAVE_CLOCK_MONOTONIC
# define CLOCK_MONOTONIC 1
#endif

#ifndef HAVE_STRUCT_TIMESPEC
struct timespec {
  long tv_sec;
  long tv_nsec;
};
#endif

#ifndef HAVE_CLOCK_GETTIME
# define P9Y_NEED_CLOCK_GETTIME
int clock_gettime(int clock_type, struct timespec *spec);
# ifdef _WIN32
# endif // _WIN32
#endif // HAVE_CLOCK_GETTIME
