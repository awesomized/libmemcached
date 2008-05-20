#include "common.h"

/* 
  "Murmur"hash provided by Austin, tanjent@gmail.com
*/

#define MIX(h,k,m) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

uint32_t murmur_hash(const char *key, size_t length)
{
  const uint32_t m= 0x5bd1e995;
  const int r= 16;
  uint32_t h= length * m;
  uint32_t k = 0;

  while(length >= 4)
  {
    k = *(uint32_t*)key;
    MIX(h,k,m);

    key += 4;
    length -= 4;
  }

  switch(length)
  {
  case 3: k += key[2] << 16;
  case 2: k += key[1] << 8;
  case 1: k += key[0];
          MIX(h,k,m);
  };

  h *= m;
  h ^= h >> 10;
  h *= m;
  h ^= h >> 17;

  return h;
}
