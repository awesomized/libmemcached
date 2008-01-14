#ifdef USE_PRAGMA_INTERFACE
#pragma interface                       /* gcc class implementation */
#endif

#include <memcached.h>
#include <stdio.h>

class Memcached
{
  memcached_st memc;

public:

 Memcached();
 Memcached(memcached_st *clone);
 ~Memcached()
 {
   memcached_free(&memc);
 }

 char *get(char *key, size_t *value_length);
 memcached_return set(char *key, char *value, size_t value_length);
};
