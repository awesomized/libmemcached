/*
  Memcached library
*/

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation        // gcc: Class implementation
#endif

#include <strings.h>
#include <memcached.hh>

Memcached::Memcached()
{
  memcached_create(&memc);
}

Memcached::Memcached(memcached_st *clone)
{
  WATCHPOINT;
  memcached_clone(&memc, clone);
  WATCHPOINT;
}

char *Memcached::get(char *key, size_t *value_length)
{
  uint32_t flags;
  memcached_return rc;

  return memcached_get(&memc, key, strlen(key),
                       value_length, &flags, &rc);
}

memcached_return Memcached::set(char *key, char *value, size_t value_length)
{
  return memcached_set(&memc, key, strlen(key), 
                       value, value_length, 
                       (time_t)0, (uint32_t)0);
}
