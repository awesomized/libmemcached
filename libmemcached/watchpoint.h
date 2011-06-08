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

#pragma once

/* Some personal debugging functions */
#if defined(DEBUG)

#ifdef TARGET_OS_LINUX
static inline void libmemcached_stack_dump(void)
{
  void *array[10];
  int size;
  char **strings;

  size= backtrace(array, 10);
  strings= backtrace_symbols(array, size);

  fprintf(stderr, "Found %d stack frames.\n", size);

  for (int x= 0; x < size; x++)
    fprintf(stderr, "%s\n", strings[x]);

  free (strings);

  fflush(stderr);
}

#elif defined(__sun)
#include <ucontext.h>

static inline void libmemcached_stack_dump(void)
{
   fflush(stderr);
   printstack(fileno(stderr));
}

#else

static inline void libmemcached_stack_dump(void)
{ }

#endif // libmemcached_stack_dump()

#include <assert.h>

#define WATCHPOINT do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s)\n", __FILE__, __LINE__,__func__);fflush(stdout); } while (0)
#define WATCHPOINT_ERROR(A) do {fprintf(stderr, "\nWATCHPOINT %s:%d %s\n", __FILE__, __LINE__, memcached_strerror(NULL, A));fflush(stdout); } while (0)
#define WATCHPOINT_IFERROR(A) do { if(A != MEMCACHED_SUCCESS)fprintf(stderr, "\nWATCHPOINT %s:%d %s\n", __FILE__, __LINE__, memcached_strerror(NULL, A));fflush(stdout); } while (0)
#define WATCHPOINT_STRING(A) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s\n", __FILE__, __LINE__,__func__,A);fflush(stdout); } while (0)
#define WATCHPOINT_STRING_LENGTH(A,B) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %.*s\n", __FILE__, __LINE__,__func__,(int)B,A);fflush(stdout); } while (0)
#define WATCHPOINT_NUMBER(A) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %lu\n", __FILE__, __LINE__,__func__,(unsigned long)(A));fflush(stdout); } while (0)
#define WATCHPOINT_LABELED_NUMBER(A,B) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s:%lu\n", __FILE__, __LINE__,__func__,(A),(unsigned long)(B));fflush(stdout); } while (0)
#define WATCHPOINT_IF_LABELED_NUMBER(A,B,C) do { if(A) {fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s:%lu\n", __FILE__, __LINE__,__func__,(B),(unsigned long)(C));fflush(stdout);} } while (0)
#define WATCHPOINT_ERRNO(A) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s\n", __FILE__, __LINE__,__func__, strerror(A));fflush(stdout); } while (0)
#define WATCHPOINT_ASSERT_PRINT(A,B,C) do { if(!(A)){fprintf(stderr, "\nWATCHPOINT ASSERT %s:%d (%s) ", __FILE__, __LINE__,__func__);fprintf(stderr, (B),(C));fprintf(stderr,"\n");fflush(stdout); libmemcached_stack_dump(); } assert((A)); } while (0)
#define WATCHPOINT_ASSERT(A) do { if (! (A)) {libmemcached_stack_dump();} assert((A)); } while (0)
#define WATCHPOINT_ASSERT_INITIALIZED(A) do { if (! (A)) { libmemcached_stack_dump(); } assert(memcached_is_initialized((A))); } while (0);
#define WATCHPOINT_SET(A) do { A; } while(0);

#else

#define WATCHPOINT
#define WATCHPOINT_ERROR(A)
#define WATCHPOINT_IFERROR(__memcached_return_t) (void)(__memcached_return_t)
#define WATCHPOINT_STRING(A)
#define WATCHPOINT_NUMBER(A)
#define WATCHPOINT_LABELED_NUMBER(A,B)
#define WATCHPOINT_IF_LABELED_NUMBER(A,B,C)
#define WATCHPOINT_ERRNO(A)
#define WATCHPOINT_ASSERT_PRINT(A,B,C)
#define WATCHPOINT_ASSERT(A) (void)(A)
#define WATCHPOINT_ASSERT_INITIALIZED(A)
#define WATCHPOINT_SET(A)

#endif /* DEBUG */
