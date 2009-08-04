/*
 * Summary: Callback function for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */
#ifndef LIBMEMCACHED_MEMCACHED_CALLBACK_H
#define LIBMEMCACHED_MEMCACHED_CALLBACK_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_fetch_execute(memcached_st *ptr, 
                                             memcached_execute_function *callback,
                                             void *context,
                                             unsigned int number_of_callbacks);

LIBMEMCACHED_API
memcached_return memcached_callback_set(memcached_st *ptr, 
                                        memcached_callback flag, 
                                        void *data);
LIBMEMCACHED_API
void *memcached_callback_get(memcached_st *ptr, 
                             memcached_callback flag,
                             memcached_return *error);
#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_CALLBACK_H */
