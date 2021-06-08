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

memcached_server_list_st memcached_server_list_append_with_weight(memcached_server_list_st ptr,
                                                                  const char *hostname,
                                                                  in_port_t port, uint32_t weight,
                                                                  memcached_return_t *error) {
  memcached_return_t unused;
  if (error == NULL) {
    error = &unused;
  }

  if (hostname == NULL) {
    hostname = "localhost";
  }

  if (hostname[0] == '/') {
    port = 0;
  } else if (port == 0) {
    port = MEMCACHED_DEFAULT_PORT;
  }

  /* Increment count for hosts */
  uint32_t count = 1;
  if (ptr) {
    count += memcached_server_list_count(ptr);
  }

  memcached_server_list_st new_host_list =
      (memcached_server_st *) realloc(ptr, sizeof(memcached_server_st) * count);
  if (new_host_list == NULL) {
#if 0
    *error= memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT);
#endif
    return NULL;
  }

  memcached_string_t _hostname = {memcached_string_make_from_cstr(hostname)};
  /* @todo Check return type */
  if (server_create_with(NULL, &new_host_list[count - 1], _hostname, port, weight,
                         port ? MEMCACHED_CONNECTION_TCP : MEMCACHED_CONNECTION_UNIX_SOCKET)
      == NULL)
  {
#if 0
    *error= memcached_set_errno(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT);
#endif
    free(new_host_list);
    return NULL;
  }

#if 0
  // Handset allocated since
  new_host_list->options.is_allocated= true;
#endif

  /* Backwards compatibility hack */
  memcached_servers_set_count(new_host_list, count);

  *error = MEMCACHED_SUCCESS;
  return new_host_list;
}

memcached_server_list_st memcached_server_list_append(memcached_server_list_st ptr,
                                                      const char *hostname, in_port_t port,
                                                      memcached_return_t *error) {
  return memcached_server_list_append_with_weight(ptr, hostname, port, 0, error);
}

uint32_t memcached_server_list_count(const memcached_server_list_st self) {
  return (self == NULL) ? 0 : self->number_of_hosts;
}

uint32_t memcached_instance_list_count(const memcached_st *self) {
  return (self == NULL) ? 0 : self->number_of_hosts;
}

void memcached_instance_set(memcached_st *memc, memcached_instance_st *list,
                            const uint32_t host_list_size) {
  assert(memc);
  memc->servers = list;
  memc->number_of_hosts = host_list_size;
}

void memcached_server_list_free(memcached_server_list_st self) {
  if (self) {
    for (uint32_t x = 0; x < memcached_server_list_count(self); x++) {
      assert_msg(not memcached_is_allocated(&self[x]),
                 "You have called memcached_server_list_free(), but you did not pass it a valid "
                 "memcached_server_list_st");
      server_free(&self[x]);
    }

    libmemcached_free(self->root, self);
  }
}

void memcached_instance_list_free(memcached_instance_st *self, uint32_t instance_count) {
  if (self) {
    for (uint32_t x = 0; x < instance_count; x++) {
      assert_msg(memcached_is_allocated(&self[x]) == false,
                 "You have called memcached_server_list_free(), but you did not pass it a valid "
                 "memcached_server_list_st");
      instance_free(&self[x]);
    }

    libmemcached_free(self->root, self);
  }
}
