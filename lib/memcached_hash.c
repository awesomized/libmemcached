#include "common.h"

unsigned int memcached_generate_hash(char *key, size_t key_length)
{
  unsigned int x = key_length;
  char *ptr = key;
  unsigned int value = 0;

  while (x--) 
  {
    value += *ptr++;
    value += (value << 10);
    value ^= (value >> 6);
  }
  value += (value << 3);
  value ^= (value >> 11);
  value += (value << 15); 

  return value == 0 ? 1 : value;
}
