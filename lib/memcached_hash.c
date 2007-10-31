#include "common.h"

/* Defines */
static uint64_t FNV_64_INIT= 0xcbf29ce484222325L;
static uint64_t FNV_64_PRIME= 0x100000001b3L;

static uint32_t FNV_32_INIT= 2166136261L;
static uint32_t FNV_32_PRIME= 16777619;

/* Prototypes */
static unsigned int internal_generate_hash(char *key, size_t key_length);
static uint32_t internal_generate_md5(char *key, size_t key_length);
static uint32_t internal_generate_md5(char *key, size_t key_length);

unsigned int memcached_generate_hash(memcached_st *ptr, char *key, size_t key_length)
{
  uint32_t hash;

  switch (ptr->hash)
  {
  case MEMCACHED_HASH_DEFAULT:
    hash= internal_generate_hash(key, key_length);
    break;
  case MEMCACHED_HASH_MD5:
    hash= internal_generate_md5(key, key_length);
    break;
  case MEMCACHED_HASH_CRC:
    hash= hash_crc32(key, key_length);
    break;
  }

  if (ptr->flags & MEM_USE_KETAMA)
  {
    WATCHPOINT_ASSERT(0);
    return 0;
  }
  else
    return hash % ptr->number_of_hosts;
}

static uint32_t internal_generate_hash(char *key, size_t key_length)
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

static uint32_t internal_generate_md5(char *key, size_t key_length)
{
  unsigned char results[16];

  md5_signature((unsigned char*)key, (unsigned int)key_length, results);

  return (uint32_t)(( results[3] << 24 )
                    | ( results[2] << 16 )
                    | ( results[1] <<  8 )
                    |   results[0] );
}
