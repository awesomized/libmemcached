/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <libmemcached/common.h>
#include <libmemcached/virtual_bucket.h>

static const memcached_st global_copy= {
  .state= {
    .is_purging= false,
    .is_processing_input= false,
    .is_time_for_rebuild= false,
  },
  .flags= {
    .auto_eject_hosts= false,
    .binary_protocol= false,
    .buffer_requests= false,
    .hash_with_prefix_key= false,
    .no_block= false,
    .no_reply= false,
    .randomize_replica_read= false,
    .support_cas= false,
    .tcp_nodelay= false,
    .use_sort_hosts= false,
    .use_udp= false,
    .verify_key= false,
    .tcp_keepalive= false,
  },
};

static inline bool _memcached_init(memcached_st *self)
{
  self->state= global_copy.state;
  self->flags= global_copy.flags;
  self->virtual_bucket= NULL;

  self->distribution= MEMCACHED_DISTRIBUTION_MODULA;

  hashkit_st *hash_ptr;
  hash_ptr= hashkit_create(&self->hashkit);
  if (! hash_ptr)
    return false;

  self->ketama.continuum= NULL;
  self->ketama.continuum_count= 0;
  self->ketama.continuum_points_counter= 0;
  self->ketama.next_distribution_rebuild= 0;
  self->ketama.weighted= false;

  self->number_of_hosts= 0;
  self->servers= NULL;
  self->last_disconnected_server= NULL;

  self->snd_timeout= 0;
  self->rcv_timeout= 0;
  self->server_failure_limit= 0;
  self->query_id= 0;

  /* TODO, Document why we picked these defaults */
  self->io_msg_watermark= 500;
  self->io_bytes_watermark= 65 * 1024;

  self->tcp_keepidle= 0;

  self->io_key_prefetch= 0;
  self->poll_timeout= MEMCACHED_DEFAULT_TIMEOUT;
  self->connect_timeout= MEMCACHED_DEFAULT_CONNECT_TIMEOUT;
  self->retry_timeout= 0;

  self->send_size= -1;
  self->recv_size= -1;

  self->user_data= NULL;
  self->number_of_replicas= 0;
  hash_ptr= hashkit_create(&self->distribution_hashkit);
  if (! hash_ptr)
    return false;

  self->allocators= memcached_allocators_return_default();

  self->on_clone= NULL;
  self->on_cleanup= NULL;
  self->get_key_failure= NULL;
  self->delete_trigger= NULL;
  self->callbacks= NULL;
  self->sasl.callbacks= NULL;
  self->sasl.is_allocated= false;

  self->error_messages= NULL;
  self->prefix_key= NULL;
  self->configure.initial_pool_size= 1;
  self->configure.max_pool_size= 1;
  self->configure.filename= NULL;

  return true;
}

static void _free(memcached_st *ptr, bool release_st)
{
  /* If we have anything open, lets close it now */
  send_quit(ptr);
  memcached_server_list_free(memcached_server_list(ptr));
  memcached_result_free(&ptr->result);

  memcached_virtual_bucket_free(ptr);

  if (ptr->last_disconnected_server)
    memcached_server_free(ptr->last_disconnected_server);

  if (ptr->on_cleanup)
    ptr->on_cleanup(ptr);

  if (ptr->ketama.continuum)
    libmemcached_free(ptr, ptr->ketama.continuum);

  memcached_array_free(ptr->prefix_key);
  ptr->prefix_key= NULL;

  memcached_error_free(ptr);

  if (ptr->sasl.callbacks)
  {
#ifdef LIBMEMCACHED_WITH_SASL_SUPPORT
    memcached_destroy_sasl_auth_data(ptr);
#endif
  }

  if (release_st)
  {
    memcached_array_free(ptr->configure.filename);
    ptr->configure.filename= NULL;
  }

  if (memcached_is_allocated(ptr) && release_st)
  {
    libmemcached_free(ptr, ptr);
  }
}

memcached_st *memcached_create(memcached_st *ptr)
{
  if (ptr == NULL)
  {
    ptr= (memcached_st *)malloc(sizeof(memcached_st));

    if (! ptr)
    {
      return NULL; /*  MEMCACHED_MEMORY_ALLOCATION_FAILURE */
    }

    ptr->options.is_allocated= true;
  }
  else
  {
    ptr->options.is_allocated= false;
  }

#if 0
  memcached_set_purging(ptr, false);
  memcached_set_processing_input(ptr, false);
#endif

  if (! _memcached_init(ptr))
  {
    memcached_free(ptr);
    return NULL;
  }

  if (! memcached_result_create(ptr, &ptr->result))
  {
    memcached_free(ptr);
    return NULL;
  }

  WATCHPOINT_ASSERT_INITIALIZED(&ptr->result);

  return ptr;
}

memcached_st *memcached(const char *string, size_t length)
{
  if (! length || ! string)
  {
    errno= EINVAL;
    return NULL;
  }

  memcached_st *self= memcached_create(NULL);
  if (! self)
  {
    errno= ENOMEM;
    return NULL;
  }

  memcached_return_t rc;
  rc= memcached_parse_configuration(self, string, length);

  if (rc == MEMCACHED_SUCCESS && memcached_parse_filename(self))
  {
    rc= memcached_parse_configure_file(self, memcached_parse_filename(self), memcached_parse_filename_length(self));
  }
    
  if (rc != MEMCACHED_SUCCESS)
  {
    memcached_free(self);
    errno= EINVAL;
    return NULL;
  }

  errno= 0;

  return self;
}

memcached_return_t memcached_reset(memcached_st *ptr)
{
  WATCHPOINT_ASSERT(ptr);
  if (! ptr)
    return MEMCACHED_INVALID_ARGUMENTS;

  bool stored_is_allocated= memcached_is_allocated(ptr);
  uint64_t query_id= ptr->query_id;
  _free(ptr, false);
  memcached_create(ptr);
  memcached_set_allocated(ptr, stored_is_allocated);
  ptr->query_id= query_id;

  if (ptr->configure.filename)
  {
    return memcached_parse_configure_file(ptr, memcached_param_array(ptr->configure.filename));
  }

  return MEMCACHED_SUCCESS;
}

void memcached_servers_reset(memcached_st *ptr)
{
  if (! ptr)
    return;

  memcached_server_list_free(memcached_server_list(ptr));

  memcached_server_list_set(ptr, NULL);
  ptr->number_of_hosts= 0;
  if (ptr->last_disconnected_server)
  {
    memcached_server_free(ptr->last_disconnected_server);
  }
  ptr->last_disconnected_server= NULL;
  ptr->server_failure_limit= 0;
}

void memcached_reset_last_disconnected_server(memcached_st *ptr)
{
  if (! ptr)
    return;

  if (ptr->last_disconnected_server)
  {
    memcached_server_free(ptr->last_disconnected_server);
    ptr->last_disconnected_server= NULL;
  }
}

void memcached_free(memcached_st *ptr)
{
  if (! ptr)
    return;

  _free(ptr, true);
}

/*
  clone is the destination, while source is the structure to clone.
  If source is NULL the call is the same as if a memcached_create() was
  called.
*/
memcached_st *memcached_clone(memcached_st *clone, const memcached_st *source)
{
  memcached_return_t rc= MEMCACHED_SUCCESS;
  memcached_st *new_clone;

  if (source == NULL)
    return memcached_create(clone);

  if (clone && memcached_is_allocated(clone))
  {
    return NULL;
  }

  new_clone= memcached_create(clone);

  if (new_clone == NULL)
    return NULL;

  new_clone->flags= source->flags;
  new_clone->send_size= source->send_size;
  new_clone->recv_size= source->recv_size;
  new_clone->poll_timeout= source->poll_timeout;
  new_clone->connect_timeout= source->connect_timeout;
  new_clone->retry_timeout= source->retry_timeout;
  new_clone->distribution= source->distribution;

  hashkit_st *hash_ptr;

  hash_ptr= hashkit_clone(&new_clone->hashkit, &source->hashkit);
  if (! hash_ptr)
  {
    memcached_free(new_clone);
    return NULL;
  }

  hash_ptr= hashkit_clone(&new_clone->distribution_hashkit, &source->distribution_hashkit);
  if (! hash_ptr)
  {
    memcached_free(new_clone);
    return NULL;
  }

  new_clone->user_data= source->user_data;

  new_clone->snd_timeout= source->snd_timeout;
  new_clone->rcv_timeout= source->rcv_timeout;

  new_clone->on_clone= source->on_clone;
  new_clone->on_cleanup= source->on_cleanup;

  new_clone->allocators= source->allocators;

  new_clone->get_key_failure= source->get_key_failure;
  new_clone->delete_trigger= source->delete_trigger;
  new_clone->server_failure_limit= source->server_failure_limit;
  new_clone->io_msg_watermark= source->io_msg_watermark;
  new_clone->io_bytes_watermark= source->io_bytes_watermark;
  new_clone->io_key_prefetch= source->io_key_prefetch;
  new_clone->number_of_replicas= source->number_of_replicas;
  new_clone->tcp_keepidle= source->tcp_keepidle;

  if (memcached_server_count(source))
    rc= memcached_push(new_clone, source);

  if (rc != MEMCACHED_SUCCESS)
  {
    memcached_free(new_clone);

    return NULL;
  }


  new_clone->prefix_key= memcached_array_clone(new_clone, source->prefix_key);

#ifdef LIBMEMCACHED_WITH_SASL_SUPPORT
  if (source->sasl.callbacks)
  {
    if (memcached_clone_sasl(new_clone, source) != MEMCACHED_SUCCESS)
    {
      memcached_free(new_clone);
      return NULL;
    }
  }
#endif

  rc= run_distribution(new_clone);

  if (rc != MEMCACHED_SUCCESS)
  {
    memcached_free(new_clone);

    return NULL;
  }

  if (source->on_clone)
    source->on_clone(new_clone, source);

  return new_clone;
}

void *memcached_get_user_data(const memcached_st *ptr)
{
  return ptr->user_data;
}

void *memcached_set_user_data(memcached_st *ptr, void *data)
{
  void *ret= ptr->user_data;
  ptr->user_data= data;

  return ret;
}

memcached_return_t memcached_push(memcached_st *destination, const memcached_st *source)
{
  return memcached_server_push(destination, source->servers);
}

memcached_server_write_instance_st memcached_server_instance_fetch(memcached_st *ptr, uint32_t server_key)
{
  return &ptr->servers[server_key];
}

memcached_server_instance_st memcached_server_instance_by_position(const memcached_st *ptr, uint32_t server_key)
{
  return &ptr->servers[server_key];
}
