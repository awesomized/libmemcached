/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Localized copy of WATCHPOINT debug symbols
 *
 */

#ifndef LIBMEMCACHED_MEMCACHED_WATCHPOINT_H
#define LIBMEMCACHED_MEMCACHED_WATCHPOINT_H

/* Some personal debugging functions */
#if defined(DEBUG)

#include <assert.h>

#define WATCHPOINT do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s)\n", __FILE__, __LINE__,__func__);fflush(stdout); } while (0)
#define WATCHPOINT_ERROR(A) do {fprintf(stderr, "\nWATCHPOINT %s:%d %s\n", __FILE__, __LINE__, memcached_strerror(NULL, A));fflush(stdout); } while (0)
#define WATCHPOINT_IFERROR(A) do { if(A != MEMCACHED_SUCCESS)fprintf(stderr, "\nWATCHPOINT %s:%d %s\n", __FILE__, __LINE__, memcached_strerror(NULL, A));fflush(stdout); } while (0)
#define WATCHPOINT_STRING(A) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s\n", __FILE__, __LINE__,__func__,A);fflush(stdout); } while (0)
#define WATCHPOINT_STRING_LENGTH(A,B) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %.*s\n", __FILE__, __LINE__,__func__,(int)B,A);fflush(stdout); } while (0)
#define WATCHPOINT_NUMBER(A) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %zu\n", __FILE__, __LINE__,__func__,(size_t)(A));fflush(stdout); } while (0)
#define WATCHPOINT_ERRNO(A) do { fprintf(stderr, "\nWATCHPOINT %s:%d (%s) %s\n", __FILE__, __LINE__,__func__, strerror(A));fflush(stdout); } while (0)
#define WATCHPOINT_ASSERT_PRINT(A,B,C) do { if(!(A)){fprintf(stderr, "\nWATCHPOINT ASSERT %s:%d (%s) ", __FILE__, __LINE__,__func__);fprintf(stderr, (B),(C));fprintf(stderr,"\n");fflush(stdout);}assert((A)); } while (0)
#define WATCHPOINT_ASSERT(A) assert((A))
#define WATCHPOINT_ASSERT_INITIALIZED(A) assert(memcached_is_initialized((A)))

#else

#define WATCHPOINT
#define WATCHPOINT_ERROR(A)
#define WATCHPOINT_IFERROR(A)
#define WATCHPOINT_STRING(A)
#define WATCHPOINT_NUMBER(A)
#define WATCHPOINT_ERRNO(A)
#define WATCHPOINT_ASSERT_PRINT(A,B,C)
#define WATCHPOINT_ASSERT(A)
#define WATCHPOINT_ASSERT_INITIALIZED(A)

#endif /* DEBUG */

#endif /* LIBMEMCACHED_MEMCACHED_WATCHPOINT_H */
