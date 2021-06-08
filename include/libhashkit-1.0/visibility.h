/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#pragma once

/**
 *
 * HASHKIT_API is used for the public API symbols. It either DLL imports or
 * DLL exports (or does nothing for static build).
 *
 * HASHKIT_LOCAL is used for non-api symbols.
 */

#if defined(BUILDING_HASHKIT)
#  if defined(HAVE_VISIBILITY) && HAVE_VISIBILITY
#    define HASHKIT_API   __attribute__((visibility("default")))
#    define HASHKIT_LOCAL __attribute__((visibility("hidden")))
#  elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#    define HASHKIT_API   __global
#    define HASHKIT_LOCAL __hidden
#  elif defined(_MSC_VER)
#    define HASHKIT_API extern __declspec(dllexport)
#    define HASHKIT_LOCAL
#  else
#    define HASHKIT_API
#    define HASHKIT_LOCAL
#  endif /* defined(HAVE_VISIBILITY) */
#else    /* defined(BUILDING_HASHKIT) */
#  if defined(_MSC_VER)
#    define HASHKIT_API extern __declspec(dllimport)
#    define HASHKIT_LOCAL
#  else
#    define HASHKIT_API
#    define HASHKIT_LOCAL
#  endif /* defined(_MSC_VER) */
#endif   /* defined(BUILDING_HASHKIT) */
