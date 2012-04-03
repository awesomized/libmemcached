AC_DEFUN([CONFIG_EXTRA], [

AH_TOP([
#pragma once

/* _SYS_FEATURE_TESTS_H is Solaris, _FEATURES_H is GCC */
#if defined( _SYS_FEATURE_TESTS_H) || defined(_FEATURES_H)
#error "You should include config.h as your first include file"
#endif

])

AH_BOTTOM([

#if defined(__cplusplus) 
#include CSTDINT_H 
#include CINTTYPES_H 
#else 
#include <stdint.h> 
#include <inttypes.h> 
#endif

#if !defined(HAVE_ULONG) && !defined(__USE_MISC)
# define HAVE_ULONG 1
typedef unsigned long int ulong;
#endif 
 
 
#if defined(RMEMCACHED_PACKAGE) 
#undef VERSION 
#define VERSION VERSION_NUMBER
#endif

#if defined(RMEMCACHED_PACKAGE)
#undef PACKAGE
#define PACKAGE RMEMCACHED_PACKAGE
#endif


])

])dnl CONFIG_EXTRA
