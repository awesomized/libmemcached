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

#include "libhashkit/common.h"

/* FNV hash'es lifted from Dustin Sallings work */
static uint32_t FNV_32_INIT = 2166136261UL;
static uint32_t FNV_32_PRIME = 16777619;

uint32_t hashkit_fnv1_32(const char *key, size_t key_length, void *context) {
  uint32_t hash = FNV_32_INIT;
  (void) context;

  for (size_t x = 0; x < key_length; x++) {
    uint32_t val = (uint32_t) key[x];
    hash *= FNV_32_PRIME;
    hash ^= val;
  }

  return hash;
}

uint32_t hashkit_fnv1a_32(const char *key, size_t key_length, void *context) {
  uint32_t hash = FNV_32_INIT;
  (void) context;

  for (size_t x = 0; x < key_length; x++) {
    uint32_t val = (uint32_t) key[x];
    hash ^= val;
    hash *= FNV_32_PRIME;
  }

  return hash;
}
