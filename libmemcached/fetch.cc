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

char *memcached_fetch(memcached_st *ptr, char *key, size_t *key_length, 
                      size_t *value_length, 
                      uint32_t *flags,
                      memcached_return_t *error)
{
  memcached_result_st *result_buffer= &ptr->result;
  memcached_return_t unused;
  if (not error)
    error= &unused;


  unlikely (ptr->flags.use_udp)
  {
    *error= MEMCACHED_NOT_SUPPORTED;
    return NULL;
  }

  result_buffer= memcached_fetch_result(ptr, result_buffer, error);

  if (result_buffer == NULL or *error != MEMCACHED_SUCCESS)
  {
    WATCHPOINT_ASSERT(result_buffer == NULL);
    *value_length= 0;
    return NULL;
  }

  *value_length= memcached_string_length(&result_buffer->value);

  if (key)
  {
    if (result_buffer->key_length > MEMCACHED_MAX_KEY)
    {
      *error= MEMCACHED_KEY_TOO_BIG;
      *value_length= 0;

      return NULL;
    }
    strncpy(key, result_buffer->item_key, result_buffer->key_length); // For the binary protocol we will cut off the key :(
    *key_length= result_buffer->key_length;
  }

  if (flags)
    *flags= result_buffer->item_flags;

  return memcached_string_c_copy(&result_buffer->value);
}

memcached_result_st *memcached_fetch_result(memcached_st *ptr,
                                            memcached_result_st *result,
                                            memcached_return_t *error)
{
  memcached_server_st *server;

  memcached_return_t unused;
  if (not error)
    error= &unused;

  if (ptr->flags.use_udp)
  {
    *error= MEMCACHED_NOT_SUPPORTED;
    return NULL;
  }

  if (not result)
  {
    if (not (result= memcached_result_create(ptr, NULL)))
    {
      *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
      return NULL;
    }
  }

  while ((server= memcached_io_get_readable_server(ptr)))
  {
    char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
    *error= memcached_response(server, buffer, sizeof(buffer), result);

    if (*error == MEMCACHED_SUCCESS)
    {
      return result;
    }
    else if (*error == MEMCACHED_END)
    {
      memcached_server_response_reset(server);
    }
    else if (*error != MEMCACHED_NOTFOUND)
    {
      break;
    }
  }

  /* We have completed reading data */
  if (memcached_is_allocated(result))
  {
    memcached_result_free(result);
  }
  else
  {
    memcached_string_reset(&result->value);
  }

  if (*error == MEMCACHED_NOTFOUND)
    *error= MEMCACHED_END;

  if (*error == MEMCACHED_SUCCESS)
    *error= MEMCACHED_END;

  return NULL;
}

memcached_return_t memcached_fetch_execute(memcached_st *ptr, 
                                           memcached_execute_fn *callback,
                                           void *context,
                                           uint32_t number_of_callbacks)
{
  memcached_result_st *result= &ptr->result;
  memcached_return_t rc= MEMCACHED_FAILURE;

  while ((result= memcached_fetch_result(ptr, result, &rc)))
  {
    if (memcached_success(rc))
    {
      for (uint32_t x= 0; x < number_of_callbacks; x++)
      {
        rc= (*callback[x])(ptr, result, context);
        if (memcached_failed(rc))
          break;
      }
    }
  }
  return rc;
}
