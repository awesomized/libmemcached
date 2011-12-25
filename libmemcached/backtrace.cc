/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libmcachedd client library.
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

#include <cstring>
#include <cstdlib>

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef HAVE_CXXABI_H
#include <cxxabi.h>
#endif

#ifdef HAVE_GCC_ABI_DEMANGLE
#define USE_DEMANGLE 1
#else
#define USE_DEMANGLE 0
#endif

void custom_backtrace(void)
{
#ifdef HAVE_EXECINFO_H
  void *array[50];

  size_t size= backtrace(array, 50);
  char **strings= backtrace_symbols(array, size);

  if (strings == NULL)
  {
    return;
  }

  fprintf(stderr, "Number of stack frames obtained: %lu\n", (unsigned long)size);

  char *named_function= (char *)::realloc(NULL, 1024);
  
  if (named_function == NULL)
  {
    ::free(strings);
    return;
  }

  for (size_t x= 1; x < size; x++) 
  {
    if (USE_DEMANGLE)
    {
      size_t sz= 200;
      char *named_function_ptr= (char *)::realloc(named_function, sz);
      if (named_function_ptr == NULL)
      {
        continue;
      }
      named_function= named_function_ptr;

      char *begin_name= 0;
      char *begin_offset= 0;
      char *end_offset= 0;

      for (char *j= strings[x]; *j; ++j)
      {
        if (*j == '(')
        {
          begin_name= j;
        }
        else if (*j == '+')
        {
          begin_offset= j;
        }
        else if (*j == ')' and begin_offset) 
        {
          end_offset= j;
          break;
        }
      }

      if (begin_name and begin_offset and end_offset and begin_name < begin_offset)
      {
        *begin_name++= '\0';
        *begin_offset++= '\0';
        *end_offset= '\0';

        int status;
        char *ret= abi::__cxa_demangle(begin_name, named_function, &sz, &status);
        if (ret) // realloc()'ed string
        {
          named_function= ret;
          fprintf(stderr, "  %s : %s()+%s\n", strings[x], begin_name, begin_offset);
        }
        else
        {
          fprintf(stderr, "  %s : %s()+%s\n", strings[x], begin_name, begin_offset);
        }
      }
      else
      {
        fprintf(stderr, " %s\n", strings[x]);
      }
    }
    else
    {
      fprintf(stderr, " unmangled: %s\n", strings[x]);
    }
  }

  ::free(named_function);
  ::free(strings);
#endif // HAVE_EXECINFO_H
}
