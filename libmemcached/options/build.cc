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
#include <libmemcached/options/context.h>
#include <libmemcached/options/build.h>

namespace parser {

void abort_func(Context *context, const char *error)
{
  if (context->rc == MEMCACHED_SUCCESS)
    context->rc= MEMCACHED_PARSE_ERROR;

  memcached_string_st *error_string= memcached_string_create(context->memc, NULL, 1024);
  memcached_string_append(error_string, memcached_string_with_size("Error occured while parsing: "));
  memcached_string_append(error_string, memcached_string_make_from_cstr(context->begin));
  memcached_string_append(error_string, memcached_string_with_size(" ("));

  if (context->rc == MEMCACHED_PARSE_ERROR and error)
  {
    memcached_string_append(error_string, memcached_string_make_from_cstr(error));
  }
  else
  {
    memcached_string_append(error_string, memcached_string_make_from_cstr(memcached_strerror(NULL, context->rc)));
  }
  memcached_string_append(error_string, memcached_string_with_size(")"));

  memcached_set_error_string(context->memc, context->rc, memcached_string_value(error_string), memcached_string_length(error_string));

  memcached_string_free(error_string);
}

} // namespace parser
