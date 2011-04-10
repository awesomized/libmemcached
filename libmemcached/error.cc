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

#include "libmemcached/common.h"

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

static void _set(memcached_st *memc, memcached_string_t *str, const memcached_return_t rc, const int local_errno)
{
  WATCHPOINT_ASSERT(memc);
  if (! memc)
    return;

  if (memc->error_messages && memc->error_messages->query_id != memc->query_id)
  {
    memcached_error_free(memc);
  }

  memcached_error_t *error;
  error= (struct memcached_error_t *)libmemcached_malloc(memc, sizeof(struct memcached_error_t));

  if (! error)
    return;

  error->root= memc;
  error->query_id= memc->query_id;
  error->rc= rc;
  error->local_errno= local_errno;

  if (str)
  {
    size_t length= str->size > (size_t)MAX_ERROR_LENGTH ? MAX_ERROR_LENGTH : str->size;
    error->size= length;
    memcpy(error->message, str->c_str, error->size);
    error->message[error->size]= 0;
  }
  else
  {
    error->size= 0;
  }

  error->next= memc->error_messages;
  memc->error_messages= error;
}

memcached_return_t memcached_set_error_string(memcached_st *memc, memcached_return_t rc, const char *str, size_t length)
{
  memcached_string_t tmp;
  tmp.c_str= str;
  tmp.size= length;
  return memcached_set_error(memc, rc, &tmp);
}

memcached_return_t memcached_set_error(memcached_st *memc, memcached_return_t rc, memcached_string_t *str)
{
  if (rc == MEMCACHED_SUCCESS)
    return MEMCACHED_SUCCESS;

  _set(memc, str, rc, 0);

  return rc;
}

memcached_return_t memcached_set_errno(memcached_st *memc, int local_errno, memcached_string_t *str)
{
  _set(memc, str, MEMCACHED_ERRNO, local_errno);

  return MEMCACHED_ERRNO;
}

static void _error_print(const memcached_error_t *error)
{
  if (! error)
    return;

  if (! error->size)
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
  if (! self)
    return;

  _error_print(self->error_messages);
}

static void _error_free(memcached_error_t *error)
{
  if (! error)
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
  if (! self)
    return;

  _error_free(self->error_messages);
}

const char *memcached_last_error_message(memcached_st *memc)
{
  if (! memc)
    return memcached_strerror(memc, MEMCACHED_INVALID_ARGUMENTS);

  if (! memc->error_messages)
    return memcached_strerror(memc, MEMCACHED_SUCCESS);

  if (! memc->error_messages->size)
  {
    return memcached_strerror(memc, memc->error_messages->rc);
  }

  return memc->error_messages->message;
}

memcached_return_t memcached_last_error(memcached_st *memc)
{
  if (! memc)
    return MEMCACHED_INVALID_ARGUMENTS;

  if (! memc->error_messages)
    return MEMCACHED_SUCCESS;

  return memc->error_messages->rc;
}

int memcached_last_error_errno(memcached_st *memc)
{
  if (! memc)
    return 0;

  if (! memc->error_messages)
    return 0;

  return memc->error_messages->local_errno;
}
