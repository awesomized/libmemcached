/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker 
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Internal functions used by the library. Not for public use!
 *
 */

#ifndef LIBMEMCACHED_MEMCACHED_INTERNAL_H
#define LIBMEMCACHED_MEMCACHED_INTERNAL_H

#if defined(BUILDING_LIBMEMCACHED)

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_LOCAL
void libmemcached_free(memcached_st *ptr, void *mem);
LIBMEMCACHED_LOCAL
void *libmemcached_malloc(memcached_st *ptr, const size_t size);
LIBMEMCACHED_LOCAL
void *libmemcached_realloc(memcached_st *ptr, void *mem, const size_t size);
LIBMEMCACHED_LOCAL
void *libmemcached_calloc(memcached_st *ptr, size_t nelem, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* BUILDING_LIBMEMCACHED */
#endif /* LIBMEMCACHED_MEMCACHED_INTERNAL_H */
