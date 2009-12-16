/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Change the behavior of the memcached connection.
 *
 */

#ifndef __MEMCACHED_BEHAVIOR_H__
#define __MEMCACHED_BEHAVIOR_H__

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return_t memcached_behavior_set(memcached_st *ptr, memcached_behavior_t flag, uint64_t data);

LIBMEMCACHED_API
uint64_t memcached_behavior_get(memcached_st *ptr, memcached_behavior_t flag);


#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_BEHAVIOR_H__ */
