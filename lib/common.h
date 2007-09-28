/*
  Common include file for libmemached
*/

#include <memcached.h>

#include <libmemcached_config.h>

#ifdef HAVE_DTRACE
#define _DTRACE_VERSION 1
#else
#undef _DTRACE_VERSION
#endif

#include "libmemcached_probes.h"
