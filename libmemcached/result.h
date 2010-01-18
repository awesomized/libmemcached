/* LibMemcached
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Functions to manipulate the result structure.
 *
 */

#ifndef __MEMCACHED_RESULT_H__
#define __MEMCACHED_RESULT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct memcached_result_st {
  struct {
    bool is_allocated:1;
    bool is_initialized:1;
  } options;
  uint32_t flags;
  time_t expiration;
  memcached_st *root;
  size_t key_length;
  uint64_t cas;
  memcached_string_st value;
  char key[MEMCACHED_MAX_KEY];
  /* Add result callback function */
};

/* Result Struct */
LIBMEMCACHED_API
void memcached_result_free(memcached_result_st *result);
LIBMEMCACHED_API
void memcached_result_reset(memcached_result_st *ptr);
LIBMEMCACHED_API
memcached_result_st *memcached_result_create(memcached_st *ptr,
                                             memcached_result_st *result);

static inline const char *memcached_result_key_value(const memcached_result_st *self)
{
  return self->key;
}

static inline size_t memcached_result_key_length(const memcached_result_st *self)
{
  return self->key_length;
}

static inline const char *memcached_result_value(const memcached_result_st *self)
{
  const memcached_string_st *sptr= &self->value;
  return memcached_string_value(sptr);
}

static inline size_t memcached_result_length(const memcached_result_st *self)
{
  const memcached_string_st *sptr= &self->value;
  return memcached_string_length(sptr);
}

static inline uint32_t memcached_result_flags(const memcached_result_st *self)
{
  return self->flags;
}

static inline uint64_t memcached_result_cas(const memcached_result_st *self)
{
  return self->cas;
}

static inline memcached_return_t memcached_result_set_value(memcached_result_st *ptr, const char *value, size_t length)
{
  return memcached_string_append(&ptr->value, value, length);
}

static inline void memcached_result_set_flags(memcached_result_st *self, uint32_t flags)
{
  self->flags= flags;
}

static inline void memcached_result_set_expiration(memcached_result_st *self, time_t expiration)
{
  self->expiration= expiration;
}

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_RESULT_H__ */
