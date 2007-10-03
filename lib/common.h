/*
  Common include file for libmemached
*/

#include <memcached.h>
#include <memcached_io.h>

#include <libmemcached_config.h>

#ifdef HAVE_DTRACE
#define _DTRACE_VERSION 1
#else
#undef _DTRACE_VERSION
#endif

#include "libmemcached_probes.h"
