/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/ 
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#pragma once

/**
 *
 * LIBMEMCACHED_API is used for the public API symbols. It either DLL imports or
 * DLL exports (or does nothing for static build).
 *
 * LIBMEMCACHED_LOCAL is used for non-api symbols.
 */

#if defined(BUILDING_LIBMEMCACHEDINTERNAL)
# if defined(HAVE_VISIBILITY) && HAVE_VISIBILITY
#  define LIBMEMCACHED_API __attribute__ ((visibility("default")))
#  define LIBMEMCACHED_LOCAL  __attribute__ ((visibility("default")))
# elif defined (__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#  define LIBMEMCACHED_API __global
#  define LIBMEMCACHED_LOCAL __global
# elif defined(_MSC_VER)
#  define LIBMEMCACHED_API extern __declspec(dllexport) 
#  define LIBMEMCACHED_LOCAL extern __declspec(dllexport)
# else
#  define LIBMEMCACHED_API
#  define LIBMEMCACHED_LOCAL
# endif
#else
# if defined(BUILDING_LIBMEMCACHED)
#  if defined(HAVE_VISIBILITY) && HAVE_VISIBILITY
#   define LIBMEMCACHED_API __attribute__ ((visibility("default")))
#   define LIBMEMCACHED_LOCAL  __attribute__ ((visibility("hidden")))
#  elif defined (__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#   define LIBMEMCACHED_API __global
#   define LIBMEMCACHED_LOCAL __hidden
#  elif defined(_MSC_VER)
#   define LIBMEMCACHED_API extern __declspec(dllexport) 
#   define LIBMEMCACHED_LOCAL
#  else
#   define LIBMEMCACHED_API
#   define LIBMEMCACHED_LOCAL
#  endif /* defined(HAVE_VISIBILITY) */
# else  /* defined(BUILDING_LIBMEMCACHED) */
#  if defined(_MSC_VER)
#   define LIBMEMCACHED_API extern __declspec(dllimport) 
#   define LIBMEMCACHED_LOCAL
#  else
#   define LIBMEMCACHED_API
#   define LIBMEMCACHED_LOCAL
#  endif /* defined(_MSC_VER) */
# endif /* defined(BUILDING_LIBMEMCACHED) */
#endif /* defined(BUILDING_LIBMEMCACHEDINTERNAL) */
