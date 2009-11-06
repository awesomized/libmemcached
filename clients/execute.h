#ifndef CLIENTS_EXECUTE_H
#define CLIENTS_EXECUTE_H
#include "libmemcached/memcached.h"
#include "generator.h"

unsigned int execute_set(memcached_st *memc, pairs_st *pairs, unsigned int number_of);
unsigned int execute_get(memcached_st *memc, pairs_st *pairs, unsigned int number_of);
unsigned int execute_mget(memcached_st *memc, const char * const *keys, size_t *key_length,
                          unsigned int number_of);
#endif

