/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "libmemcached/common.h"
#include <sys/types.h>

/*
  These functions provide data and function callback support
*/

memcached_return_t memcached_callback_set(memcached_st *shell, const memcached_callback_t flag,
                                          const void *data) {
  Memcached *ptr = memcached2Memcached(shell);
  if (ptr) {
    switch (flag) {
    case MEMCACHED_CALLBACK_PREFIX_KEY: {
      return memcached_set_namespace(*ptr, (char *) data, data ? strlen((char *) data) : 0);
    }

    case MEMCACHED_CALLBACK_USER_DATA: {
      ptr->user_data = const_cast<void *>(data);
      break;
    }

    case MEMCACHED_CALLBACK_CLEANUP_FUNCTION: {
      memcached_cleanup_fn func = *(memcached_cleanup_fn *) &data;
      ptr->on_cleanup = func;
      break;
    }

    case MEMCACHED_CALLBACK_CLONE_FUNCTION: {
      memcached_clone_fn func = *(memcached_clone_fn *) &data;
      ptr->on_clone = func;
      break;
    }

    case MEMCACHED_CALLBACK_GET_FAILURE: {
      memcached_trigger_key_fn func = *(memcached_trigger_key_fn *) &data;
      ptr->get_key_failure = func;
      break;
    }

    case MEMCACHED_CALLBACK_DELETE_TRIGGER: {
      if (data) // NULL would mean we are disabling.
      {
        if (memcached_behavior_get(ptr, MEMCACHED_BEHAVIOR_BUFFER_REQUESTS)) {
          return memcached_set_error(
              *ptr, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
              memcached_literal_param("Delete triggers cannot be used if buffering is enabled"));
        }

        if (memcached_behavior_get(ptr, MEMCACHED_BEHAVIOR_NOREPLY)) {
          return memcached_set_error(
              *ptr, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
              memcached_literal_param(
                  "Delete triggers cannot be used if MEMCACHED_BEHAVIOR_NOREPLY is set"));
        }
      }

      memcached_trigger_delete_key_fn func = *(memcached_trigger_delete_key_fn *) &data;
      ptr->delete_trigger = func;
      break;
    }

    case MEMCACHED_CALLBACK_MAX:
      return memcached_set_error(*ptr, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
                                 memcached_literal_param("Invalid callback supplied"));
    }

    return MEMCACHED_SUCCESS;
  }

  return MEMCACHED_INVALID_ARGUMENTS;
}

void *memcached_callback_get(memcached_st *shell, const memcached_callback_t flag,
                             memcached_return_t *error) {
  Memcached *ptr = memcached2Memcached(shell);
  memcached_return_t local_error;
  if (error == NULL) {
    error = &local_error;
  }

  if (ptr == NULL) {
    *error = MEMCACHED_INVALID_ARGUMENTS;
    return NULL;
  }

  switch (flag) {
  case MEMCACHED_CALLBACK_PREFIX_KEY: {
    *error = MEMCACHED_SUCCESS;
    if (ptr->_namespace) {
      return (void *) memcached_array_string(ptr->_namespace);
    }
    return NULL;
  }

  case MEMCACHED_CALLBACK_USER_DATA: {
    *error = ptr->user_data ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
    return (void *) ptr->user_data;
  }

  case MEMCACHED_CALLBACK_CLEANUP_FUNCTION: {
    *error = ptr->on_cleanup ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
    return *(void **) &ptr->on_cleanup;
  }

  case MEMCACHED_CALLBACK_CLONE_FUNCTION: {
    *error = ptr->on_clone ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
    return *(void **) &ptr->on_clone;
  }

  case MEMCACHED_CALLBACK_GET_FAILURE: {
    *error = ptr->get_key_failure ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
    return *(void **) &ptr->get_key_failure;
  }

  case MEMCACHED_CALLBACK_DELETE_TRIGGER: {
    *error = ptr->delete_trigger ? MEMCACHED_SUCCESS : MEMCACHED_FAILURE;
    return *(void **) &ptr->delete_trigger;
  }

  case MEMCACHED_CALLBACK_MAX:
    break;
  }

  assert_msg(0, "Invalid callback passed to memcached_callback_get()");
  *error = MEMCACHED_FAILURE;
  return NULL;
}
