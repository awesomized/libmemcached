#include "common.h"

void libmemcached_free(memcached_st *ptr, void *mem)
{
  (void) ptr;
  free(mem);
}

void *libmemcached_malloc(memcached_st *ptr, size_t size)
{
  (void) ptr;
  return malloc(size);
}

void *libmemcached_realloc(memcached_st *ptr, void *mem, size_t size)
{
  (void) ptr;
  return realloc(mem, size);
}

void *libmemcached_calloc(memcached_st *ptr, size_t nelem, size_t size)
{
  if (ptr->call_malloc != libmemcached_malloc)
  {
     void *ret = libmemcached_malloc(ptr, nelem * size);
     if (ret != NULL) 
       memset(ret, 0, nelem * size);

     return ret;
  }

  return calloc(nelem, size);
}

memcached_return_t memcached_set_memory_allocators(memcached_st *ptr,
                                                   memcached_malloc_fn mem_malloc,
                                                   memcached_free_fn mem_free,
                                                   memcached_realloc_fn mem_realloc,
                                                   memcached_calloc_fn mem_calloc)
{
  /* All should be set, or none should be set */
  if (mem_malloc == NULL && mem_free == NULL && mem_realloc == NULL && mem_calloc == NULL) 
  {
    ptr->call_malloc= libmemcached_malloc;
    ptr->call_free= libmemcached_free;
    ptr->call_realloc= libmemcached_realloc;
    ptr->call_calloc= libmemcached_calloc;
  }
  else if (mem_malloc == NULL || mem_free == NULL || mem_realloc == NULL || mem_calloc == NULL)
    return MEMCACHED_FAILURE;
  else
  {
    ptr->call_malloc= mem_malloc;
    ptr->call_free= mem_free;
    ptr->call_realloc= mem_realloc;
    ptr->call_calloc= mem_calloc;
  }

  return MEMCACHED_SUCCESS;
}

void memcached_get_memory_allocators(memcached_st *ptr,
                                     memcached_malloc_fn *mem_malloc,
                                     memcached_free_fn *mem_free,
                                     memcached_realloc_fn *mem_realloc,
                                     memcached_calloc_fn *mem_calloc)
{
   *mem_malloc= ptr->call_malloc;
   *mem_free= ptr->call_free;
   *mem_realloc= ptr->call_realloc;
   *mem_calloc= ptr->call_calloc;
}
