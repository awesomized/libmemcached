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

/*
  This is a partial implementation for fetching/creating memcached_server_st objects.
*/
#include <libmemcached/common.h>
#include <cassert>

static inline void _server_init(memcached_server_st *self, memcached_st *root,
                                const char *hostname, in_port_t port,
                                uint32_t weight, memcached_connection_t type)
{
  self->options.is_shutting_down= false;
  self->options.is_dead= false;
  self->number_of_hosts= 0;
  self->cursor_active= 0;
  self->port= port;
  self->cached_errno= 0;
  self->fd= -1;
  self->io_bytes_sent= 0;
  self->server_failure_counter= 0;
  self->weight= weight ? weight : 1; // 1 is the default weight value
  WATCHPOINT_SET(self->io_wait_count.read= 0);
  WATCHPOINT_SET(self->io_wait_count.write= 0);
  self->major_version= UINT8_MAX;
  self->micro_version= UINT8_MAX;
  self->minor_version= UINT8_MAX;
  self->type= type;
  self->read_ptr= self->read_buffer;
  self->cached_server_error= NULL;
  self->read_buffer_length= 0;
  self->read_data_length= 0;
  self->write_buffer_offset= 0;
  self->address_info= NULL;
  self->address_info_next= NULL;

  self->state= MEMCACHED_SERVER_STATE_NEW;

  if (root)
  {
    self->next_retry= root->retry_timeout;
  }
  else
  {
    self->next_retry= 0;
  }

  self->root= root;
  self->limit_maxbytes= 0;
  if (hostname)
  {
    strncpy(self->hostname, hostname, NI_MAXHOST - 1);
  }
  else
  {
    self->hostname[0]= 0;
  }
}

static memcached_server_st *_server_create(memcached_server_st *self, const memcached_st *memc)
{
  if (not self)
  {
   self= (memcached_server_st *)libmemcached_malloc(memc, sizeof(memcached_server_st));

    if (not self)
      return NULL; /*  MEMCACHED_MEMORY_ALLOCATION_FAILURE */

    self->options.is_allocated= true;
  }
  else
  {
    self->options.is_allocated= false;
  }

  self->options.is_initialized= true;

  return self;
}

memcached_server_st *memcached_server_create_with(const memcached_st *memc,
                                                  memcached_server_write_instance_st self,
                                                  const char *hostname, in_port_t port,
                                                  uint32_t weight, memcached_connection_t type)
{
  self= _server_create(self, memc);

  if (not self)
    return NULL;

  _server_init(self, const_cast<memcached_st *>(memc), hostname, port, weight, type);


  if (type == MEMCACHED_CONNECTION_UDP)
  {
    self->write_buffer_offset= UDP_DATAGRAM_HEADER_LENGTH;
    memcached_io_init_udp_header(self, 0);
  }

  return self;
}

void memcached_server_free(memcached_server_st *self)
{
  if (not self)
    return;

  memcached_quit_server(self, false);

  if (self->cached_server_error)
    free(self->cached_server_error);

  if (self->address_info)
    freeaddrinfo(self->address_info);

  if (memcached_is_allocated(self))
  {
    libmemcached_free(self->root, self);
  }
  else
  {
    self->options.is_initialized= false;
  }
}

/*
  If we do not have a valid object to clone from, we toss an error.
*/
memcached_server_st *memcached_server_clone(memcached_server_st *destination,
                                            const memcached_server_st *source)
{
  /* We just do a normal create if source is missing */
  if (not source)
    return NULL;

  destination= memcached_server_create_with(source->root, destination,
                                            source->hostname, source->port, source->weight,
                                            source->type);
  if (not destination)
  {
    destination->cached_errno= source->cached_errno;

    if (source->cached_server_error)
      destination->cached_server_error= strdup(source->cached_server_error);
  }

  return destination;

}

memcached_return_t memcached_server_cursor(const memcached_st *ptr,
                                           const memcached_server_fn *callback,
                                           void *context,
                                           uint32_t number_of_callbacks)
{
  memcached_return_t rc;
  if (memcached_failed(rc= initialize_const_query(ptr)))
  {
    return rc;
  }

  for (uint32_t x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_server_instance_st instance=
      memcached_server_instance_by_position(ptr, x);

    for (uint32_t y= 0; y < number_of_callbacks; y++)
    {
      unsigned int iferror;

      iferror= (*callback[y])(ptr, instance, context);

      if (iferror)
        continue;
    }
  }

  return MEMCACHED_SUCCESS;
}

memcached_return_t memcached_server_execute(memcached_st *ptr,
                                            memcached_server_execute_fn callback,
                                            void *context)
{
  for (uint32_t x= 0; x < memcached_server_count(ptr); x++)
  {
    memcached_server_write_instance_st instance= memcached_server_instance_fetch(ptr, x);

    unsigned int iferror= (*callback)(ptr, instance, context);

    if (iferror)
      continue;
  }

  return MEMCACHED_SUCCESS;
}

memcached_server_instance_st memcached_server_by_key(const memcached_st *ptr,
                                                     const char *key,
                                                     size_t key_length,
                                                     memcached_return_t *error)
{
  memcached_return_t rc;
  memcached_return_t unused;

  if (not error)
    error= &unused;

  if (memcached_failed(rc= initialize_const_query(ptr)))
  {
    *error= rc;
    return NULL;
  }

  if (memcached_failed(rc= memcached_validate_key_length(key_length, ptr->flags.binary_protocol)))
  {
    *error= rc;
    return NULL;
  }

  if (ptr->flags.verify_key && (memcached_key_test((const char **)&key, &key_length, 1) == MEMCACHED_BAD_KEY_PROVIDED))
  {
    *error= MEMCACHED_BAD_KEY_PROVIDED;
    return NULL;
  }

  uint32_t server_key= memcached_generate_hash(ptr, key, key_length);
  return memcached_server_instance_by_position(ptr, server_key);

}

void memcached_server_error_reset(memcached_server_st *self)
{
  WATCHPOINT_ASSERT(self);
  if (not self)
    return;

  self->cached_server_error[0]= 0;
}

memcached_server_instance_st memcached_server_get_last_disconnect(const memcached_st *self)
{
  WATCHPOINT_ASSERT(self);
  if (not self)
    return 0;

  return self->last_disconnected_server;
}

void memcached_server_list_free(memcached_server_list_st self)
{
  if (not self)
    return;

  for (uint32_t x= 0; x < memcached_server_list_count(self); x++)
  {
    if (self[x].address_info)
    {
      freeaddrinfo(self[x].address_info);
      self[x].address_info= NULL;
    }
  }

  libmemcached_free(self->root, self);
}

uint32_t memcached_servers_set_count(memcached_server_st *servers, uint32_t count)
{
  WATCHPOINT_ASSERT(servers);
  if (not servers)
    return 0;

  return servers->number_of_hosts= count;
}

uint32_t memcached_server_count(const memcached_st *self)
{
  WATCHPOINT_ASSERT(self);
  if (not self)
    return 0;

  return self->number_of_hosts;
}

const char *memcached_server_name(memcached_server_instance_st self)
{
  WATCHPOINT_ASSERT(self);
  if (not self)
    return NULL;

  return self->hostname;
}

in_port_t memcached_server_port(memcached_server_instance_st self)
{
  WATCHPOINT_ASSERT(self);
  if (not self)
    return 0;

  return self->port;
}

uint32_t memcached_server_response_count(memcached_server_instance_st self)
{
  WATCHPOINT_ASSERT(self);
  if (not self)
    return 0;

  return self->cursor_active;
}

const char *memcached_server_error(memcached_server_instance_st ptr)
{
  return ptr ?  ptr->cached_server_error : NULL;
}

