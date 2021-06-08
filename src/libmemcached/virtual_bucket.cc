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
#include "libmemcached/virtual_bucket.h"

struct bucket_t {
  uint32_t master;
  uint32_t forward;
};

struct memcached_virtual_bucket_t {
  bool has_forward;
  uint32_t size;
  uint32_t replicas;
  struct bucket_t buckets[];
};

memcached_return_t memcached_virtual_bucket_create(memcached_st *self, const uint32_t *host_map,
                                                   const uint32_t *forward_map,
                                                   const uint32_t buckets,
                                                   const uint32_t replicas) {
  if (self == NULL || host_map == NULL || buckets == 0U) {
    return MEMCACHED_INVALID_ARGUMENTS;
  }

  memcached_virtual_bucket_free(self);

  struct memcached_virtual_bucket_t *virtual_bucket = (struct memcached_virtual_bucket_t *) malloc(
      sizeof(struct memcached_virtual_bucket_t) + sizeof(struct bucket_t) * buckets);

  if (virtual_bucket == NULL) {
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
  }

  virtual_bucket->size = buckets;
  virtual_bucket->replicas = replicas;
  self->virtual_bucket = virtual_bucket;

  uint32_t x = 0;
  for (; x < buckets; x++) {
    virtual_bucket->buckets[x].master = host_map[x];
    if (forward_map) {
      virtual_bucket->buckets[x].forward = forward_map[x];
    } else {
      virtual_bucket->buckets[x].forward = 0;
    }
  }

  return MEMCACHED_SUCCESS;
}

void memcached_virtual_bucket_free(memcached_st *self) {
  if (self) {
    if (self->virtual_bucket) {
      free(self->virtual_bucket);
      self->virtual_bucket = NULL;
    }
  }
}

uint32_t memcached_virtual_bucket_get(const memcached_st *self, uint32_t digest) {
  if (self) {
    if (self->virtual_bucket) {
      uint32_t result = (uint32_t)(digest & (self->virtual_bucket->size - 1));
      return self->virtual_bucket->buckets[result].master;
    }

    return (uint32_t)(digest & (self->number_of_hosts - 1));
  }

  return 0;
}
