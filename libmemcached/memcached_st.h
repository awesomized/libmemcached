/*
 * Summary: Common functions to operate on the memcached_st for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#ifndef LIBMEMCACHED_MEMCACHED_ST_H
#define LIBMEMCACHED_MEMCACHED_ST_H

#ifndef LIBMEMCACHED_MEMCACHED_H
#error "Please include <libmemcached/memcached.h> instead"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LIBMEMCACHED_API
memcached_return memcached_version(memcached_st *ptr);

LIBMEMCACHED_API
const char * memcached_lib_version(void);

LIBMEMCACHED_API
memcached_st *memcached_create(memcached_st *ptr);
LIBMEMCACHED_API
void memcached_free(memcached_st *ptr);
LIBMEMCACHED_API
memcached_st *memcached_clone(memcached_st *clone, memcached_st *ptr);

LIBMEMCACHED_API
memcached_return memcached_verbosity(memcached_st *ptr, unsigned int verbosity);
LIBMEMCACHED_API
void memcached_quit(memcached_st *ptr);
LIBMEMCACHED_API
const char *memcached_strerror(memcached_st *ptr, memcached_return rc);

LIBMEMCACHED_API
void *memcached_get_user_data(memcached_st *ptr);
LIBMEMCACHED_API
void *memcached_set_user_data(memcached_st *ptr, void *data);

LIBMEMCACHED_API
memcached_return run_distribution(memcached_st *ptr);

#ifdef __cplusplus
}
#endif

#endif /* LIBMEMCACHED_MEMCACHED_ST_H */
