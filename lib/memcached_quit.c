#include <memcached.h>

/*
  When this is implemented you will be able to remove single hosts
  from your current pool of hosts.
*/
memcached_return memcached_quit(memcached_st *ptr, char *hostname, unsigned port)
{
  return MEMCACHED_SUCCESS;
}
