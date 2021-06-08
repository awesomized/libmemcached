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
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#ifndef CLIENTS_MS_ATOMIC_H
#define CLIENTS_MS_ATOMIC_H

#include "mem_config.h"

#if defined(__SUNPRO_C) || defined(HAVE_ATOMIC_ADD_NV)
#  define ATOMIC volatile
#  define _KERNEL
#  include <atomic.h>
#  if SIZEOF_SIZE_T == 8
#    define atomic_add_size(X, Y) atomic_add_64((X), (Y))
#  else
#    define atomic_add_size(X, Y) atomic_add_32((X), (Y))
#  endif
#  undef _KERNEL

#elif HAVE_C_STDATOMIC
#  define ATOMIC _Atomic
#  include <stdatomic.h>
#  define atomic_dec_32(X)        atomic_fetch_sub(X,1)
#  define atomic_add_32           atomic_fetch_add
#  define atomic_add_32_nv(X,Y)  (atomic_fetch_add(X,Y)+(Y))
#  define atomic_add_size         atomic_fetch_add

#elif HAVE_BUILTIN_ATOMIC
#  define ATOMIC
#  define atomic_dec_32(X)  BUILTIN_ATOMIC_PREFIX ## _atomic_fetch_sub(X,1)
#  define atomic_add_32     BUILTIN_ATOMIC_PREFIX ## _atomic_fetch_add
#  define atomic_add_32_nv  BUILTIN_ATOMIC_PREFIX ## _atomic_add_fetch(X,Y)
#  define atomic_add_size   BUILTIN_ATOMIC_PREFIX ## _atomic_fetch_add

#elif HAVE_BUILTIN_SYNC
#  define ATOMIC
#  define atomic_dec_32(X)  __sync_fetch_and_sub((X), 1)
#  define atomic_add_32     __sync_fetch_and_add
#  define atomic_add_32_nv  __sync_add_and_fetch
#  define atomic_add_size   __sync_fetch_and_add

#else
#  warning "Atomic operators not found so memslap will not work correctly"
#  define ATOMIC
#  define atomic_dec_32(X)    (--(*(X)))
#  define atomic_add_32(X,Y)  ((*(X))+=(Y))
#  define atomic_add_32_nv    atomic_add_32
#  define atomic_add_size     atomic_add_32

#endif

#endif /* CLIENTS_MS_ATOMIC_H */
