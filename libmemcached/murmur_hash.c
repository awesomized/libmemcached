#include "common.h"

/* 
  "Murmur"hash provided by Austin, tanjent@gmail.com
#define MURMUR_POSTMIX
*/

uint32_t murmur_hash(char *key, size_t length)
{
  const unsigned int m= 0x7fd652ad;
  const int r= 16;

  uint32_t h= 0xdeadbeef;

  while(length >= 4)
  {
    h += *(unsigned int *)key;
    h *= m;
    h ^= h >> r;

    key += 4;
    length -= 4;
  }

  switch(length)
  {
  case 3:
    h += key[2] << 16;
  case 2:
    h += key[1] << 8;
  case 1:
  default:
    h += key[0];
    h *= m;
    h ^= h >> r;
  };

#ifdef MURMUR_POSTMIX

  h *= m;
  h ^= h >> 10;
  h *= m;
  h ^= h >> 17;

#endif

  return h;
}
