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

#include "libhashkit/hashkitcon.h"

#include "libhashkit/murmur3.h"

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

#ifdef __GNUC__
#  define FORCE_INLINE __attribute__((always_inline)) inline
#else
#  define FORCE_INLINE inline
#endif

static FORCE_INLINE uint32_t rotl32(uint32_t x, int8_t r) {
  return (x << r) | (x >> (32 - r));
}

#define ROTL32(x, y) rotl32(x, y)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

#include <cassert>
#include <cstring>
template<typename T>
static inline T getblock(const T *blocks, int i) {
  T b;
  memcpy(&b, ((const uint8_t *) blocks) + i * sizeof(T), sizeof(T));
  return b;
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t fmix32(uint32_t h) {
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out) {
  const uint8_t *data = (const uint8_t *) key;
  const int nblocks = len / 4;
  int i;

  uint32_t h1 = seed;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t *blocks = (const uint32_t *) (data + nblocks * 4);

  for (i = -nblocks; i; i++) {
    uint32_t k1 = getblock(blocks, i);
#if WORDS_BIGENDIAN
    k1 = BYTESWAP_32(k1);
#endif

    k1 *= c1;
    k1 = ROTL32(k1, 15);
    k1 *= c2;

    h1 ^= k1;
    h1 = ROTL32(h1, 13);
    h1 = h1 * 5 + 0xe6546b64;
  }

  //----------
  // tail

  const uint8_t *tail = (const uint8_t *) (data + nblocks * 4);

  uint32_t k1 = 0;
  memcpy(&k1, tail, len & 3);
#if WORDS_BIGENDIAN
  k1 = BYTESWAP_32(k1);
#endif

  k1 *= c1;
  k1 = ROTL32(k1, 15);
  k1 *= c2;
  h1 ^= k1;

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix32(h1);

  *(uint32_t *) out = h1;
}
