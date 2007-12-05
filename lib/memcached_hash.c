#include "common.h"

/* Defines */
static uint64_t FNV_64_INIT= 0xcbf29ce484222325LL;
static uint64_t FNV_64_PRIME= 0x100000001b3LL;

static uint32_t FNV_32_INIT= 2166136261UL;
static uint32_t FNV_32_PRIME= 16777619;

/* Prototypes */
static uint64_t internal_generate_hash(char *key, size_t key_length);
static uint64_t internal_generate_md5(char *key, size_t key_length);
static uint64_t internal_generate_ketama_md5(char *key, size_t key_length);

unsigned int memcached_generate_hash(memcached_st *ptr, char *key, size_t key_length)
{
  uint64_t hash= 1; /* Just here to remove compile warning */
  unsigned int x;

  WATCHPOINT_ASSERT(ptr->number_of_hosts);

  switch (ptr->hash)
  {
  case MEMCACHED_HASH_DEFAULT:
    hash= internal_generate_hash(key, key_length);
    break;
  case MEMCACHED_HASH_MD5:
    hash= internal_generate_md5(key, key_length);
    break;
  case MEMCACHED_HASH_CRC:
    hash= ((hash_crc32(key, key_length) >> 16) & 0x7fff);
    break;
    /* FNV hash'es lifted from Dustin Sallings work */
  case MEMCACHED_HASH_FNV1_64: 
    {
      /* Thanks to pierre@demartines.com for the pointer */
      hash= FNV_64_INIT;
      for (x= 0; x < key_length; x++) 
      {
        hash *= FNV_64_PRIME;
        hash ^= key[x];
      }
    }
    break;
  case MEMCACHED_HASH_FNV1A_64: 
    {
      hash= FNV_64_INIT;
      for (x= 0; x < key_length; x++) 
      {
        hash ^= key[x];
        hash *= FNV_64_PRIME;
      }
    }
    break;
  case MEMCACHED_HASH_FNV1_32: 
    {
      hash= FNV_32_INIT;
      for (x= 0; x < key_length; x++) 
      {
        hash *= FNV_32_PRIME;
        hash ^= key[x];
      }
    }
    break;
  case MEMCACHED_HASH_FNV1A_32: 
    {
      hash= FNV_32_INIT;
      for (x= 0; x < key_length; x++) 
      {
        hash ^= key[x];
        hash *= FNV_32_PRIME;
      }
    }
    break;
    case MEMCACHED_HASH_KETAMA: 
    {
      hash= internal_generate_ketama_md5(key, key_length);
      break;
    }
    case MEMCACHED_HASH_HSIEH:
    {
      hash= hsieh_hash(key, key_length);
      break;
    }
  }

  WATCHPOINT_ASSERT(hash);

  if (ptr->distribution == MEMCACHED_DISTRIBUTION_MODULA)
  {
    return hash % ptr->number_of_hosts;
  }
  else
  {
    unsigned int server_key;

    server_key= hash % MEMCACHED_WHEEL_SIZE;

    return ptr->wheel[server_key];
  }
}

static uint64_t internal_generate_hash(char *key, size_t key_length)
{
  char *ptr= key;
  uint64_t value= 0;

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

static uint64_t internal_generate_md5(char *key, size_t key_length)
{
  unsigned char results[16];

  md5_signature((unsigned char*)key, (unsigned int)key_length, results);

  return (uint64_t)(( results[3] << 24 )
                    | ( results[2] << 16 )
                    | ( results[1] <<  8 )
                    |   results[0] );
}

static uint64_t internal_generate_ketama_md5(char *key, size_t key_length)
{
  unsigned char results[16];

  md5_signature((unsigned char*)key, (unsigned int)key_length, results);

  return ((uint32_t) (results[3] & 0xFF) << 24)
    | ((uint32_t) (results[2] & 0xFF) << 16)
    | ((uint32_t) (results[1] & 0xFF) << 8)
    | (results[0] & 0xFF);
}
