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

#if __WORDSIZE == 64 && defined(HAVE_FNV64_HASH)

/* FNV hash'es lifted from Dustin Sallings work */
static uint64_t FNV_64_INIT = 0xcbf29ce484222325;
static uint64_t FNV_64_PRIME = 0x100000001b3;

uint32_t hashkit_fnv1_64(const char *key, size_t key_length, void *) {
  /* Thanks to pierre@demartines.com for the pointer */
  uint64_t hash = FNV_64_INIT;

  for (size_t x = 0; x < key_length; x++) {
    hash *= FNV_64_PRIME;
    hash ^= (uint64_t) key[x];
  }

  return (uint32_t) hash;
}

uint32_t hashkit_fnv1a_64(const char *key, size_t key_length, void *) {
  uint32_t hash = (uint32_t) FNV_64_INIT;

  for (size_t x = 0; x < key_length; x++) {
    uint32_t val = (uint32_t) key[x];
    hash ^= val;
    hash *= (uint32_t) FNV_64_PRIME;
  }

  return hash;
}

#else
uint32_t hashkit_fnv1_64(const char *, size_t, void *) {
  return 0;
}

uint32_t hashkit_fnv1a_64(const char *, size_t, void *) {
  return 0;
}
#endif
