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

#include "mem_config.h"

#include "libmemcached/common.h"

#ifdef __cplusplus
#  include <cstddef>
#  include <cstdlib>
#else
#  include <stddef.h>
#  include <stdlib.h>
#endif

static inline void libmemcached_free(const memcached_st *self, void *mem) {
  if (self) {
    self->allocators.free(self, mem, self->allocators.context);
  } else if (mem) {
#ifdef __cplusplus
    std::free(mem);
#else
    free(mem);
#endif
  }
}

static inline void *libmemcached_malloc(const memcached_st *self, const size_t size) {
  if (self) {
    return self->allocators.malloc(self, size, self->allocators.context);
  }

#ifdef __cplusplus
  return std::malloc(size);
#else
  return malloc(size);
#endif
}
#define libmemcached_xmalloc(__memcachd_st, __type) \
  ((__type *) libmemcached_malloc((__memcachd_st), sizeof(__type)))

static inline void *libmemcached_realloc(const memcached_st *self, void *mem, size_t nmemb,
                                         const size_t size) {
  if (self) {
    return self->allocators.realloc(self, mem, nmemb * size, self->allocators.context);
  }

#ifdef __cplusplus
  return std::realloc(mem, size);
#else
  return realloc(mem, size);
#endif
}
#define libmemcached_xrealloc(__memcachd_st, __mem, __nelem, __type) \
  ((__type *) libmemcached_realloc((__memcachd_st), (__mem), (__nelem), sizeof(__type)))
#define libmemcached_xvalloc(__memcachd_st, __nelem, __type) \
  ((__type *) libmemcached_realloc((__memcachd_st), NULL, (__nelem), sizeof(__type)))

static inline void *libmemcached_calloc(const memcached_st *self, size_t nelem, size_t size) {
  if (self) {
    return self->allocators.calloc(self, nelem, size, self->allocators.context);
  }

#ifdef __cplusplus
  return std::calloc(nelem, size);
#else
  return calloc(nelem, size);
#endif
}
#define libmemcached_xcalloc(__memcachd_st, __nelem, __type) \
  ((__type *) libmemcached_calloc((__memcachd_st), (__nelem), sizeof(__type)))
