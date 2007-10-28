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

memcached_st *memcached_clone(memcached_st *clone, memcached_st *ptr)
{
  memcached_return rc;
  memcached_st *new_clone;

  if (ptr == NULL)
    return memcached_create(clone);
  
  new_clone= memcached_create(clone);


  rc= memcached_server_push(new_clone, ptr->hosts);

  if (rc != MEMCACHED_SUCCESS)
  {
    memcached_free(new_clone);

    return NULL;
  }


  new_clone->flags= ptr->flags;
  new_clone->number_of_hosts= ptr->number_of_hosts;
  new_clone->send_size= ptr->send_size;
  new_clone->recv_size= ptr->recv_size;

  return new_clone;
}
