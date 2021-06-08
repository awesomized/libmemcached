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

#ifdef HAVE_MURMUR_HASH

#  ifdef BYTESWAP_HEADER
#    include BYTESWAP_HEADER
#  endif

#  include <cstring>

uint32_t hashkit_murmur(const char *key, size_t length, void *context) {
  /*
    'm' and 'r' are mixing constants generated offline.  They're not
    really 'magic', they just happen to work well.
  */

  const unsigned int m = 0x5bd1e995;
  const uint32_t seed = (0xdeadbeef * (uint32_t) length);
  const int r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ (uint32_t) length;

  // Mix 4 bytes at a time into the hash

  const unsigned char *data = (const unsigned char *) key;
  (void) context;

  while (length >= 4) {
    uint32_t k;
    memcpy(&k, data, sizeof(k));
#  if WORDS_BIGENDIAN
    k = BYTESWAP_32(k);
#  endif

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    length -= 4;
  }

  // Handle the last few bytes of the input array
  if (length) {
    uint32_t k = 0;
    memcpy(&k, data, length);
#  if WORDS_BIGENDIAN
    k = BYTESWAP_32(k);
#  endif
    h ^= k;
    h *= m;
  }

  /*
    Do a few final mixes of the hash to ensure the last few bytes are
    well-incorporated.
  */

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

#else
uint32_t hashkit_murmur(const char *, size_t, void *) {
  return 0;
}
#endif
