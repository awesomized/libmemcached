/*
  Memcached library
*/
#include "common.h"

memcached_st *memcached_create(memcached_st *ptr)
{
  if (!ptr)
  {
    ptr= (memcached_st *)malloc(sizeof(memcached_st));

    if (!ptr)
      return NULL; /*  MEMCACHED_MEMORY_ALLOCATION_FAILURE */

    memset(ptr, 0, sizeof(memcached_st));
    ptr->is_allocated= MEMCACHED_ALLOCATED;
  }
  else
  {
    memset(ptr, 0, sizeof(memcached_st));
  }

  return ptr;
}

void memcached_free(memcached_st *ptr)
{
  if (ptr->hosts)
  {
    memcached_quit(ptr);
    memcached_server_list_free(ptr->hosts);
    ptr->hosts= NULL;
  }

  if (ptr->is_allocated == MEMCACHED_ALLOCATED)
    free(ptr);
  else
    memset(ptr, 0, sizeof(memcached_st));
}
