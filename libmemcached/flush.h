/* LibMemcached
 * Copyright (C) 2010 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Flush connections.
 *
 */

#ifndef __MEMCACHED_FLUSH_H__
#define __MEMCACHED_FLUSH_H__

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return_t memcached_flush(memcached_st *ptr, time_t expiration);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_FLUSH_H__ */
