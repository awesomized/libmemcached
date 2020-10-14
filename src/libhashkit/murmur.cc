/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  HashKit library
 *
 *  Copyright (C) 2011-2012 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2009 Brian Aker All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
  "Murmur" hash provided by Austin, tanjent@gmail.com
  http://murmurhash.googlepages.com/

  Note - This code makes a few assumptions about how your machine behaves -

  1. We can read a 4-byte value from any address without crashing
  2. sizeof(int) == 4

  And it has a few limitations -
  1. It will not work incrementally.
  2. It will not produce the same results on little-endian and big-endian
  machines.

  Updated to murmur2 hash - BP
*/

#include "libhashkit/common.h"

#ifdef HAVE_MURMUR_HASH

#ifdef BYTESWAP_HEADER
# include BYTESWAP_HEADER
#endif

#include <cstring>

uint32_t hashkit_murmur(const char *key, size_t length, void *context)
{
  /*
    'm' and 'r' are mixing constants generated offline.  They're not
    really 'magic', they just happen to work well.
  */

  const unsigned int m= 0x5bd1e995;
  const uint32_t seed= (0xdeadbeef * (uint32_t)length);
  const int r= 24;


  // Initialize the hash to a 'random' value

  uint32_t h= seed ^ (uint32_t)length;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data= (const unsigned char *)key;
  (void)context;

  while(length >= 4)
  {
    uint32_t k;
    memcpy(&k, data, sizeof(k));
#if WORDS_BIGENDIAN
    k = BYTESWAP_32(k);
#endif

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
#if WORDS_BIGENDIAN
    k = BYTESWAP_32(k);
#endif
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
uint32_t hashkit_murmur(const char *, size_t , void *)
{
  return 0;
}
#endif
