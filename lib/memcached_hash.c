#include "common.h"

/* Prototypes */
static unsigned int internal_generate_hash(char *key, size_t key_length);

unsigned int memcached_generate_hash(memcached_st *ptr, char *key, size_t key_length)
{
  unsigned int hash;

  if (ptr->flags & MEM_USE_MD5)
  {
    unsigned char results[16];

    md5_signature((unsigned char*)key, (unsigned int)key_length, results);

    hash= (unsigned int)(( results[3] << 24 )
				 | ( results[2] << 16 )
				 | ( results[1] <<  8 )
				 |   results[0] );
  }
  else
    hash= internal_generate_hash(key, key_length);

  if (ptr->flags & MEM_USE_KETAMA)
  {
    WATCHPOINT_ASSERT(0);
    return 0;
  }
  else
    return hash % ptr->number_of_hosts;
}

static unsigned int internal_generate_hash(char *key, size_t key_length)
{
  char *ptr= key;
  unsigned int value= 0;

  while (--key_length) 
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
