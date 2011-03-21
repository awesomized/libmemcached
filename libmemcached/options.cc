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

#include "common.h"

#include <libmemcached/options/parser.h>
#include <libmemcached/options/scanner.h>

int libmemcached_parse(type_st *, yyscan_t *);

memcached_return_t memcached_parse_options(memcached_st *self, char const *option_string, size_t length)
{
  type_st pp;

  memset(&pp, 0, sizeof(type_st));

  WATCHPOINT_ASSERT(self);
  if (! self)
    return MEMCACHED_INVALID_ARGUMENTS;

  pp.buf= option_string;
  pp.memc= self;
  pp.length= length;
  libmemcached_lex_init(&pp.yyscanner);
  libmemcached_set_extra(&pp, pp.yyscanner);
  bool success= libmemcached_parse(&pp, pp.yyscanner)  == 0;
  libmemcached_lex_destroy(pp.yyscanner);

  if (not success)
    return MEMCACHED_INVALID_ARGUMENTS;

  return MEMCACHED_SUCCESS;
}

memcached_return_t memcached_parse_file_options(memcached_st *self, const char *filename)
{
  FILE *fp= fopen(filename, "r");
  
  if (! fp)
    return memcached_set_errno(self, errno, NULL);

  char buffer[BUFSIZ];
  memcached_return_t rc= MEMCACHED_INVALID_ARGUMENTS;
  while (fgets(buffer, sizeof(buffer), fp))
  {
    size_t length= strlen(buffer);
    
    if (length == 1 and buffer[0] == '\n')
      continue;

    rc= memcached_parse_options(self, buffer, length);

    if (rc != MEMCACHED_SUCCESS)
      break;
  }
  fclose(fp);

  return rc;
}
