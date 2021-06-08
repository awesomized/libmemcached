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

static inline void _server_init(memcached_server_st *self, Memcached *root,
                                const memcached_string_t &hostname, in_port_t port, uint32_t weight,
                                memcached_connection_t type) {
  self->options.is_shutting_down = false;
  self->options.is_dead = false;
  self->number_of_hosts = 0;
  self->cursor_active = 0;
  self->port = port;
  self->io_bytes_sent = 0;
  self->request_id = 0;
  self->server_failure_counter = 0;
  self->server_failure_counter_query_id = 0;
  self->server_timeout_counter = 0;
  self->server_timeout_counter_query_id = 0;
  self->weight = weight ? weight : 1; // 1 is the default weight value
  self->io_wait_count.read = 0;
  self->io_wait_count.write = 0;
  self->io_wait_count.timeouts = 0;
  self->io_wait_count._bytes_read = 0;
  self->major_version = UINT8_MAX;
  self->micro_version = UINT8_MAX;
  self->minor_version = UINT8_MAX;
  self->type = type;
  self->error_messages = NULL;

  self->state = MEMCACHED_SERVER_STATE_NEW;
  self->next_retry = 0;

  self->root = root;
  if (root) {
    self->version = ++root->server_info.version;
  } else {
    self->version = UINT_MAX;
  }
  self->limit_maxbytes = 0;
  memcpy(self->hostname, hostname.c_str, hostname.size);
  self->hostname[hostname.size] = 0;
}

static memcached_server_st *_server_create(memcached_server_st *self, const Memcached *memc) {
  if (self == NULL) {
    self = libmemcached_xmalloc(memc, struct memcached_server_st);

    if (self == NULL) {
      return NULL; /*  MEMCACHED_MEMORY_ALLOCATION_FAILURE */
    }

    self->options.is_allocated = true;
  } else {
    self->options.is_allocated = false;
  }

  self->options.is_initialized = true;

  return self;
}

memcached_server_st *server_create_with(Memcached *memc, memcached_server_st *self,
                                          const memcached_string_t &hostname, const in_port_t port,
                                          uint32_t weight, const memcached_connection_t type) {
  if (memcached_is_valid_servername(hostname) == false) {
    memcached_set_error(*memc, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
                        memcached_literal_param("Invalid hostname provided"));
    return NULL;
  }

  self = _server_create(self, memc);

  if (self == NULL) {
    return NULL;
  }

  _server_init(self, const_cast<Memcached *>(memc), hostname, port, weight, type);

  return self;
}

void server_free(memcached_server_st *self) {
  memcached_error_free(*self);

  if (memcached_is_allocated(self)) {
    libmemcached_free(self->root, self);
  } else {
    self->options.is_initialized = false;
  }
}

void memcached_server_free(memcached_server_st *self) {
  if (self == NULL) {
    return;
  }

  if (memcached_server_list_count(self)) {
    memcached_server_list_free(self);
    return;
  }

  server_free(self);
}

void memcached_server_error_reset(memcached_server_st *self) {
  WATCHPOINT_ASSERT(self);
  if (self == NULL) {
    return;
  }

  memcached_error_free(*self);
}

uint32_t memcached_servers_set_count(memcached_server_st *servers, uint32_t count) {
  WATCHPOINT_ASSERT(servers);
  if (servers == NULL) {
    return 0;
  }

  return servers->number_of_hosts = count;
}

uint32_t memcached_server_count(const memcached_st *self) {
  WATCHPOINT_ASSERT(self);
  if (self == NULL)
    return 0;

  return self->number_of_hosts;
}

const char *memcached_server_name(const memcached_instance_st *self) {
  WATCHPOINT_ASSERT(self);
  if (self) {
    return self->_hostname;
  }

  return NULL;
}

in_port_t memcached_server_port(const memcached_instance_st *self) {
  WATCHPOINT_ASSERT(self);
  if (self == NULL) {
    return 0;
  }

  return self->port();
}

in_port_t memcached_server_srcport(const memcached_instance_st *self) {
  WATCHPOINT_ASSERT(self);
  if (self == NULL || self->fd == INVALID_SOCKET
      || (self->type != MEMCACHED_CONNECTION_TCP && self->type != MEMCACHED_CONNECTION_UDP))
  {
    return 0;
  }

  struct sockaddr_in sin;
  socklen_t addrlen = sizeof(sin);
  if (getsockname(self->fd, (struct sockaddr *) &sin, &addrlen) != -1) {
    return ntohs(sin.sin_port);
  }

  return -1;
}

uint32_t memcached_server_response_count(const memcached_instance_st *self) {
  WATCHPOINT_ASSERT(self);
  if (self == NULL) {
    return 0;
  }

  return self->cursor_active_;
}

const char *memcached_server_type(const memcached_instance_st *ptr) {
  if (ptr) {
    switch (ptr->type) {
    case MEMCACHED_CONNECTION_TCP:
      return "TCP";

    case MEMCACHED_CONNECTION_UDP:
      return "UDP";

    case MEMCACHED_CONNECTION_UNIX_SOCKET:
      return "SOCKET";
    }
  }

  return "UNKNOWN";
}

uint8_t memcached_server_major_version(const memcached_instance_st *instance) {
  if (instance) {
    return instance->major_version;
  }

  return UINT8_MAX;
}

uint8_t memcached_server_minor_version(const memcached_instance_st *instance) {
  if (instance) {
    return instance->minor_version;
  }

  return UINT8_MAX;
}

uint8_t memcached_server_micro_version(const memcached_instance_st *instance) {
  if (instance) {
    return instance->micro_version;
  }

  return UINT8_MAX;
}
