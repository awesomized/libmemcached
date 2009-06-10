/*
 * Summary: Internal functions used by the library. Not for public use!
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye
 */

#if !defined(MEMCACHED_INTERNAL_H) && defined(MEMCACHED_INTERNAL)
#define MEMCACHED_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

void libmemcached_free(memcached_st *ptr __attribute__((unused)), void *mem);
void *libmemcached_malloc(memcached_st *ptr __attribute__((unused)), 
                          const size_t size);
void *libmemcached_realloc(memcached_st *ptr __attribute__((unused)), 
                           void *mem, const size_t size);

#ifdef __cplusplus
}
#endif

#endif /* MEMCACHED_INTERNAL_H */
