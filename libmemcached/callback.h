/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Change any of the possible callbacks.
 *
 */

#ifndef __MEMCACHED_CALLBACK_H__
#define __MEMCACHED_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return_t memcached_callback_set(memcached_st *ptr,
                                          const memcached_callback_t flag,
                                          void *data);
LIBMEMCACHED_API
void *memcached_callback_get(memcached_st *ptr,
                             const memcached_callback_t flag,
                             memcached_return_t *error);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_CALLBACK_H__ */
