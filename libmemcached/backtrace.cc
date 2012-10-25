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

#ifdef HAVE_EXECINFO_H
#  include <execinfo.h>
#endif

#ifdef HAVE_GCC_ABI_DEMANGLE
#  include <cxxabi.h>
#endif

#ifdef HAVE_DLFCN_H
#  include <dlfcn.h>
#endif

#ifdef HAVE_GCC_ABI_DEMANGLE
#  define USE_DEMANGLE 1
#else
#  define USE_DEMANGLE 0
#endif

const int MAX_DEPTH= 50;

void custom_backtrace(void)
{
#ifdef HAVE_EXECINFO_H
  void *array[MAX_DEPTH];

  int backtrace_size= backtrace(array, MAX_DEPTH);
  fprintf(stderr, "Number of stack frames obtained: %d\n", backtrace_size);

#ifdef HAVE_DLFCN_H
  Dl_info dlinfo;
#endif

  for (int x= 0; x < backtrace_size; ++x)
  {  
#ifdef HAVE_DLFCN_H
    if (dladdr(array[x], &dlinfo) == 0)
    {
      continue;
    }
#endif

    const char* symname= dlinfo.dli_sname;

    int status;
    char* demangled= abi::__cxa_demangle(symname, NULL, 0, &status);
    if (status == 0 and demangled)
    {
      symname= demangled;
    }

    printf("object: %s, function: %s\n", dlinfo.dli_fname, symname);

    if (demangled)
    {
      free(demangled);
    }
  } 
#endif // HAVE_EXECINFO_H
}
