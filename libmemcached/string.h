/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: String structure used for libmemcached.
 *
 */

#ifndef __MEMCACHED_STRING_H__
#define __MEMCACHED_STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

struct memcached_string_st {
  memcached_st *root;
  char *end;
  char *string;
  size_t current_size;
  size_t block_size;
  struct {
    bool is_allocated:1;
    bool is_initialized:1;
  } options;
};

#define memcached_string_length(A) (size_t)((A)->end - (A)->string)
#define memcached_string_set_length(A, B) (A)->end= (A)->string + B
#define memcached_string_size(A) (A)->current_size
#define memcached_string_value(A) (A)->string

LIBMEMCACHED_LOCAL
memcached_string_st *memcached_string_create(memcached_st *ptr,
                                             memcached_string_st *string,
                                             size_t initial_size);
LIBMEMCACHED_LOCAL
memcached_return_t memcached_string_check(memcached_string_st *string, size_t need);

LIBMEMCACHED_LOCAL
char *memcached_string_c_copy(memcached_string_st *string);

LIBMEMCACHED_LOCAL
memcached_return_t memcached_string_append_character(memcached_string_st *string,
                                                     char character);
LIBMEMCACHED_LOCAL
memcached_return_t memcached_string_append(memcached_string_st *string,
                                           const char *value, size_t length);
LIBMEMCACHED_LOCAL
memcached_return_t memcached_string_reset(memcached_string_st *string);

LIBMEMCACHED_LOCAL
void memcached_string_free(memcached_string_st *string);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_STRING_H__ */
