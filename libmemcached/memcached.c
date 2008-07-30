/*
  Memcached library
*/
#include "common.h"

memcached_st *memcached_create(memcached_st *ptr)
{
  memcached_result_st *result_ptr;

  if (ptr == NULL)
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
  result_ptr= memcached_result_create(ptr, &ptr->result);
  WATCHPOINT_ASSERT(result_ptr);
  ptr->poll_timeout= MEMCACHED_DEFAULT_TIMEOUT;
  ptr->connect_timeout= MEMCACHED_DEFAULT_TIMEOUT;
  ptr->retry_timeout= 0;
  ptr->distribution= MEMCACHED_DISTRIBUTION_MODULA;

  return ptr;
}

void memcached_free(memcached_st *ptr)
{
  /* If we have anything open, lets close it now */
  memcached_quit(ptr);
  server_list_free(ptr, ptr->hosts);
  memcached_result_free(&ptr->result);

  if (ptr->on_cleanup)
    ptr->on_cleanup(ptr);

  if (ptr->continuum)
  {
    if (ptr->call_free)
      ptr->call_free(ptr, ptr->continuum);
    else
      free(ptr->continuum);
  }

  if (ptr->is_allocated == MEMCACHED_ALLOCATED)
  {
    if (ptr->call_free)
      ptr->call_free(ptr, ptr);
    else
      free(ptr);
  }
  else
    ptr->is_allocated= MEMCACHED_USED;
}

/*
  clone is the destination, while ptr is the structure to clone.
  If ptr is NULL the call is the same as if a memcached_create() was
  called.
*/
memcached_st *memcached_clone(memcached_st *clone, memcached_st *ptr)
{
  memcached_return rc= MEMCACHED_SUCCESS;
  memcached_st *new_clone;

  if (ptr == NULL)
    return memcached_create(clone);

  if (ptr->is_allocated == MEMCACHED_USED)
  {
    WATCHPOINT_ASSERT(0);
    return NULL;
  }
  
  new_clone= memcached_create(clone);
  
  if (new_clone == NULL)
    return NULL;

  if (ptr->hosts)
    rc= memcached_server_push(new_clone, ptr->hosts);

  if (rc != MEMCACHED_SUCCESS)
  {
    memcached_free(new_clone);

    return NULL;
  }


  new_clone->flags= ptr->flags;
  new_clone->send_size= ptr->send_size;
  new_clone->recv_size= ptr->recv_size;
  new_clone->poll_timeout= ptr->poll_timeout;
  new_clone->connect_timeout= ptr->connect_timeout;
  new_clone->retry_timeout= ptr->retry_timeout;
  new_clone->distribution= ptr->distribution;
  new_clone->hash= ptr->hash;
  new_clone->hash_continuum= ptr->hash_continuum;
  new_clone->user_data= ptr->user_data;

  new_clone->snd_timeout= ptr->snd_timeout;
  new_clone->rcv_timeout= ptr->rcv_timeout;

  new_clone->on_clone= ptr->on_clone;
  new_clone->on_cleanup= ptr->on_cleanup;
  new_clone->call_free= ptr->call_free;
  new_clone->call_malloc= ptr->call_malloc;
  new_clone->call_realloc= ptr->call_realloc;
  new_clone->get_key_failure= ptr->get_key_failure;
  new_clone->delete_trigger= ptr->delete_trigger;

  if (ptr->prefix_key[0] != 0)
  {
    strcpy(new_clone->prefix_key, ptr->prefix_key);
    new_clone->prefix_key_length= ptr->prefix_key_length;
  }

  rc= run_distribution(new_clone);
  if (rc != MEMCACHED_SUCCESS)
  {
    memcached_free(new_clone);

    return NULL;
  }

  if (ptr->on_clone)
    ptr->on_clone(ptr, new_clone);

  return new_clone;
}
