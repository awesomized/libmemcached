/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  LibMemcached
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  All rights reserved.
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
#include <cassert>

#define MAX_ERROR_LENGTH 2048
struct memcached_error_t
{
  memcached_st *root;
  uint64_t query_id;
  struct memcached_error_t *next;
  memcached_return_t rc;
  int local_errno;
  size_t size;
  char message[MAX_ERROR_LENGTH];
};

static void _set(memcached_st& memc, memcached_string_t *str, memcached_return_t &rc, const char *at, int local_errno= 0)
{
  (void)at;
  if (memc.error_messages && memc.error_messages->query_id != memc.query_id)
  {
    memcached_error_free(&memc);
  }

  // For memory allocation we use our error since it is a bit more specific
  if (local_errno == ENOMEM and rc == MEMCACHED_ERRNO)
  {
    rc= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
  }

  if (rc == MEMCACHED_MEMORY_ALLOCATION_FAILURE)
  {
    local_errno= ENOMEM;
  }

  if (rc == MEMCACHED_ERRNO and not local_errno)
  {
    local_errno= errno;
    rc= MEMCACHED_ERRNO;
  }

  if (rc == MEMCACHED_ERRNO and local_errno == ENOTCONN)
  {
    rc= MEMCACHED_CONNECTION_FAILURE;
  }

  if (local_errno == EINVAL)
  {
    rc= MEMCACHED_INVALID_ARGUMENTS;
  }

  if (local_errno == ECONNREFUSED)
  {
    rc= MEMCACHED_CONNECTION_FAILURE;
  }

  memcached_error_t *error= (struct memcached_error_t *)libmemcached_malloc(&memc, sizeof(struct memcached_error_t));
  if (not error) // Bad business if this happens
    return;

  error->root= &memc;
  error->query_id= memc.query_id;
  error->rc= rc;
  error->local_errno= local_errno;

  const char *errmsg_ptr;
  char errmsg[MAX_ERROR_LENGTH];
  errmsg[0]= 0;
  errmsg_ptr= errmsg;

  if (local_errno)
  {
#ifdef STRERROR_R_CHAR_P
    errmsg_ptr= strerror_r(local_errno, errmsg, sizeof(errmsg));
#else
    strerror_r(local_errno, errmsg, sizeof(errmsg));
    errmsg_ptr= errmsg;
#endif
  }


  if (str and str->size and local_errno)
  {
    error->size= (int)snprintf(error->message, MAX_ERROR_LENGTH, "%s(%s), %.*s -> %s", 
                               memcached_strerror(&memc, rc), 
                               errmsg_ptr,
                               memcached_string_printf(*str), at);
  }
  else if (local_errno)
  {
    error->size= (int)snprintf(error->message, MAX_ERROR_LENGTH, "%s(%s) -> %s", 
                               memcached_strerror(&memc, rc), 
                               errmsg_ptr,
                               at);
  }
  else if (str and str->size)
  {
    error->size= (int)snprintf(error->message, MAX_ERROR_LENGTH, "%s, %.*s -> %s", 
                               memcached_strerror(&memc, rc), 
                               int(str->size), str->c_str, at);
  }
  else
  {
    error->size= (int)snprintf(error->message, MAX_ERROR_LENGTH, "%s -> %s", 
                               memcached_strerror(&memc, rc), at);
  }

  error->next= memc.error_messages;
  memc.error_messages= error;
}

memcached_return_t memcached_set_error(memcached_st& memc, memcached_return_t rc, const char *at, const char *str, size_t length)
{
  assert(rc != MEMCACHED_ERRNO);
  memcached_string_t tmp= { str, length };
  return memcached_set_error(memc, rc, at, tmp);
}

memcached_return_t memcached_set_error(memcached_server_st& self, memcached_return_t rc, const char *at, const char *str, size_t length)
{
  assert(rc != MEMCACHED_ERRNO);
  memcached_string_t tmp= { str, length };
  return memcached_set_error(self, rc, at, tmp);
}

memcached_return_t memcached_set_error(memcached_st& memc, memcached_return_t rc, const char *at, memcached_string_t& str)
{
  assert(rc != MEMCACHED_ERRNO);
  if (memcached_success(rc))
    return MEMCACHED_SUCCESS;

  _set(memc, &str, rc, at);

  return rc;
}

memcached_return_t memcached_set_error(memcached_server_st& self, memcached_return_t rc, const char *at, memcached_string_t& str)
{
  assert(rc != MEMCACHED_ERRNO);
  if (memcached_success(rc))
    return MEMCACHED_SUCCESS;

  char hostname_port_message[MAX_ERROR_LENGTH];
  int size;
  if (str.size)
  {
    size= snprintf(hostname_port_message, sizeof(hostname_port_message), "%.*s, host: %s:%d",
                   memcached_string_printf(str),
                   self.hostname, int(self.port));
  }
  else
  {
    size= snprintf(hostname_port_message, sizeof(hostname_port_message), "host: %s:%d",
                   self.hostname, int(self.port));
  }

  memcached_string_t error_host= { hostname_port_message, size };

  if (not self.root)
    return rc;

  _set(*self.root, &error_host, rc, at);

  return rc;
}

memcached_return_t memcached_set_error(memcached_server_st& self, memcached_return_t rc, const char *at)
{
  assert(rc != MEMCACHED_ERRNO);
  if (memcached_success(rc))
    return MEMCACHED_SUCCESS;

  char hostname_port[NI_MAXHOST +NI_MAXSERV + sizeof("host : ")];
  int size= snprintf(hostname_port, sizeof(hostname_port), "host: %s:%d", self.hostname, int(self.port));

  memcached_string_t error_host= { hostname_port, size};

  if (not self.root)
    return rc;

  _set(*self.root, &error_host, rc, at);

  return rc;
}

memcached_return_t memcached_set_error(memcached_st& self, memcached_return_t rc, const char *at)
{
  assert(rc != MEMCACHED_ERRNO);
  if (memcached_success(rc))
    return MEMCACHED_SUCCESS;

  _set(self, NULL, rc, at);

  return rc;
}

memcached_return_t memcached_set_errno(memcached_st& self, int local_errno, const char *at, const char *str, size_t length)
{
  memcached_string_t tmp= { str, length };
  return memcached_set_errno(self, local_errno, at, tmp);
}

memcached_return_t memcached_set_errno(memcached_server_st& self, int local_errno, const char *at, const char *str, size_t length)
{
  memcached_string_t tmp= { str, length };
  return memcached_set_errno(self, local_errno, at, tmp);
}

memcached_return_t memcached_set_errno(memcached_st& self, int local_errno, const char *at)
{
  if (not local_errno)
    return MEMCACHED_SUCCESS;

  memcached_return_t rc= MEMCACHED_ERRNO;
  _set(self, NULL, rc, at, local_errno);

  return rc;
}

memcached_return_t memcached_set_errno(memcached_st& memc, int local_errno, const char *at, memcached_string_t& str)
{
  if (not local_errno)
    return MEMCACHED_SUCCESS;

  memcached_return_t rc= MEMCACHED_ERRNO;
  _set(memc, &str, rc, at, local_errno);

  return rc;
}

memcached_return_t memcached_set_errno(memcached_server_st& self, int local_errno, const char *at, memcached_string_t& str)
{
  if (not local_errno)
    return MEMCACHED_SUCCESS;

  char hostname_port_message[MAX_ERROR_LENGTH];
  int size;
  if (str.size)
  {
    size= snprintf(hostname_port_message, sizeof(hostname_port_message), "%.*s, host: %s:%d",
                   memcached_string_printf(str),
                   self.hostname, int(self.port));
  }
  else
  {
    size= snprintf(hostname_port_message, sizeof(hostname_port_message), "host: %s:%d",
                   self.hostname, int(self.port));
  }

  memcached_string_t error_host= { hostname_port_message, size };

  self.cached_errno= local_errno; // Store in the actual server

  memcached_return_t rc= MEMCACHED_ERRNO;
  if (not self.root)
    return rc;

  _set(*self.root, &error_host, rc, at, local_errno);

  return rc;
}

memcached_return_t memcached_set_errno(memcached_server_st& self, int local_errno, const char *at)
{
  if (not local_errno)
    return MEMCACHED_SUCCESS;

  char hostname_port_message[MAX_ERROR_LENGTH];
  int size = snprintf(hostname_port_message, sizeof(hostname_port_message), "host: %s:%d",
                      self.hostname, int(self.port));

  memcached_string_t error_host= { hostname_port_message, size };

  self.cached_errno= local_errno; // Store in the actual server

  memcached_return_t rc= MEMCACHED_ERRNO;
  if (not self.root)
    return rc;

  _set(*self.root, &error_host, rc, at, local_errno);

  return rc;
}

static void _error_print(const memcached_error_t *error)
{
  if (not error)
    return;

  if (not error->size)
  {
    fprintf(stderr, "%s\n", memcached_strerror(NULL, error->rc) );
  }
  else
  {
    fprintf(stderr, "%s %s\n", memcached_strerror(NULL, error->rc), error->message);
  }

  _error_print(error->next);
}

void memcached_error_print(const memcached_st *self)
{
  if (not self)
    return;

  _error_print(self->error_messages);
}

static void _error_free(memcached_error_t *error)
{
  if (not error)
    return;

  _error_free(error->next);

  if (error && error->root)
  {
    libmemcached_free(error->root, error);
  }
  else if (error)
  {
    free(error);
  }
}

void memcached_error_free(memcached_st *self)
{
  if (not self)
    return;

  _error_free(self->error_messages);
  self->error_messages= NULL;
}

const char *memcached_last_error_message(memcached_st *memc)
{
  if (not memc)
    return memcached_strerror(memc, MEMCACHED_INVALID_ARGUMENTS);

  if (not memc->error_messages)
    return memcached_strerror(memc, MEMCACHED_SUCCESS);

  if (not memc->error_messages->size)
    return memcached_strerror(memc, memc->error_messages->rc);

  return memc->error_messages->message;
}


bool memcached_has_current_error(memcached_st &memc)
{
  if (memc.error_messages 
      and memc.error_messages->query_id == memc.query_id
      and memcached_failed(memc.error_messages->rc))
  {
    return true;
  }

  return false;
}

memcached_return_t memcached_last_error(memcached_st *memc)
{
  if (not memc)
    return MEMCACHED_INVALID_ARGUMENTS;

  if (not memc->error_messages)
    return MEMCACHED_SUCCESS;

  return memc->error_messages->rc;
}

int memcached_last_error_errno(memcached_st *memc)
{
  if (not memc)
    return 0;

  if (not memc->error_messages)
    return 0;

  return memc->error_messages->local_errno;
}
