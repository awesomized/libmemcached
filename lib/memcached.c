/*
  Memcached library
*/
#include <memcached.h>

memcached_st *memcached_init(memcached_st *ptr)
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

void memcached_deinit(memcached_st *ptr)
{
  unsigned int x;
  memcached_host_st *host_ptr;

  if (ptr->hosts)
  {
    for (x= 0; x < ptr->number_of_hosts; x++)
    {
      if (ptr->hosts[x].fd == -1)
        close(ptr->hosts[x].fd);
      free(ptr->hosts[x].hostname);
    }

    free(ptr->hosts);
  }

  if (ptr->is_allocated == MEMCACHED_ALLOCATED)
    free(ptr);
  else
    memset(ptr, 0, sizeof(memcached_st));
}
