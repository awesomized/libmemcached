/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
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

#include <libmemcached/options/context.h>

const char *memcached_parse_filename(memcached_st *memc)
{
  return memcached_array_string(memc->configure.filename);
}

size_t memcached_parse_filename_length(memcached_st *memc)
{
  return memcached_array_size(memc->configure.filename);
}

static memcached_return_t _parse_file_options(memcached_st *self, memcached_string_t *filename)
{
  memcached_array_st *real_name= memcached_strcpy(self, filename->c_str, filename->size);

  if (not real_name)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  FILE *fp= fopen(memcached_array_string(real_name), "r");
  if (! fp)
  {
    memcached_string_t error_message= memcached_array_to_string(real_name);
    memcached_return_t rc=  memcached_set_errno(self, errno, &error_message);
    memcached_array_free(real_name);
    return rc;
  }
  memcached_array_free(real_name);

  char buffer[BUFSIZ];
  memcached_return_t rc= MEMCACHED_INVALID_ARGUMENTS;
  while (fgets(buffer, sizeof(buffer), fp))
  {
    size_t length= strlen(buffer);
    
    if (length == 1 and buffer[0] == '\n')
      continue;

    rc= memcached_parse_configuration(self, buffer, length);
    if (rc != MEMCACHED_SUCCESS)
      break;
  }
  fclose(fp);

  return rc;
}

memcached_return_t libmemcached_check_configuration(const char *option_string, size_t length, char *error_buffer, size_t error_buffer_size)
{
  memcached_st memc, *memc_ptr;

  if (error_buffer_size)
    error_buffer[0]= 0;

  if (! (memc_ptr= memcached_create(&memc)))
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  memcached_return_t rc= memcached_parse_configuration(memc_ptr, option_string, length);
  if (rc != MEMCACHED_SUCCESS && error_buffer && error_buffer_size)
  {
    strncpy(error_buffer, memcached_last_error_message(memc_ptr), error_buffer_size);
  }

  if (rc== MEMCACHED_SUCCESS && memcached_behavior_get(memc_ptr, MEMCACHED_BEHAVIOR_LOAD_FROM_FILE))
  {
    memcached_string_t filename= memcached_array_to_string(memc_ptr->configure.filename);
    rc= _parse_file_options(memc_ptr, &filename);

    if (rc != MEMCACHED_SUCCESS && error_buffer && error_buffer_size)
    {
      strncpy(error_buffer, memcached_last_error_message(memc_ptr), error_buffer_size);
    }
  }

  memcached_free(memc_ptr);

  return rc;
}

memcached_return_t memcached_parse_configuration(memcached_st *self, char const *option_string, size_t length)
{
  WATCHPOINT_ASSERT(self);
  if (! self)
  {
    return memcached_set_error(self, MEMCACHED_INVALID_ARGUMENTS, NULL);
  }

  memcached_return_t rc;
  Context context(option_string, length, self, rc);

  context.start();

  return rc;
}

void memcached_set_configuration_file(memcached_st *self, const char *filename, size_t filename_length)
{
  memcached_array_free(self->configure.filename);
  self->configure.filename= memcached_strcpy(self, filename, filename_length);
}

memcached_return_t memcached_parse_configure_file(memcached_st *self, const char *filename, size_t filename_length)
{
  if (! self)
    return memcached_set_error(self, MEMCACHED_INVALID_ARGUMENTS, NULL);

  if (! filename || filename_length == 0)
    return memcached_set_error(self, MEMCACHED_INVALID_ARGUMENTS, NULL);
  
  memcached_string_t tmp;
  tmp.c_str= filename;
  tmp.size= filename_length;

  return _parse_file_options(self, &tmp);
}
