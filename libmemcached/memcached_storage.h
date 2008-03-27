/*
 * Summary: Storage functions for libmemcached
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Brian Aker
 */

#ifndef __MEMCACHED_STORAGE_H__
#define __MEMCACHED_STORAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SET_OP,
  REPLACE_OP,
  ADD_OP,
  PREPEND_OP,
  APPEND_OP,
  CAS_OP,
} memcached_storage_action;

/* This is not available for public use. */
memcached_return memcached_send(memcached_st *ptr, 
                                char *master_key, size_t master_key_length, 
                                char *key, size_t key_length, 
                                char *value, size_t value_length, 
                                time_t expiration,
                                uint32_t flags,
                                uint64_t cas,
                                memcached_storage_action verb);


/* All of the functions/macros for adding data to the server */

#define memcached_set(PTR, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (KEY), (KEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, SET_OP)

#define memcached_add(PTR, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (KEY), (KEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, ADD_OP)

#define memcached_replace(PTR, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (KEY), (KEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, REPLACE_OP)

#define memcached_prepend(PTR, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (KEY), (KEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, PREPEND_OP)

#define memcached_append(PTR, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (KEY), (KEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, APPEND_OP)

#define memcached_cas(PTR, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS, CAS) \
  memcached_send((PTR), (KEY), (KEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), (CAS), CAS_OP)

#define memcached_set_by_key(PTR, MKEY, MKEY_LEN, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (MKEY), (MKEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, SET_OP)

#define memcached_add_by_key(PTR, MKEY, MKEY_LEN, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (MKEY), (MKEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, ADD_OP)

#define memcached_replace_by_key(PTR, MKEY, MKEY_LEN, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (MKEY), (MKEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, REPLACE_OP)

#define memcached_prepend_by_key(PTR, MKEY, MKEY_LEN, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (MKEY), (MKEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, PREPEND_OP)

#define memcached_append_by_key(PTR, MKEY, MKEY_LEN, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS) \
  memcached_send((PTR), (MKEY), (MKEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), 0, APPEND_OP)

#define memcached_cas_by_key(PTR, MKEY, MKEY_LEN, KEY, KEY_LEN, VALUE, VALUE_LEN, EXP, FLAGS, CAS) \
  memcached_send((PTR), (MKEY), (MKEY_LEN), (KEY), (KEY_LEN), (VALUE), (VALUE_LEN), (EXP), (FLAGS), (CAS), CAS_OP)

#ifdef __cplusplus
}
#endif

#endif /* __MEMCACHED_STORAGE_H__ */
