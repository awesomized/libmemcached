#include "common.h"

memcached_return_t memcached_key_test(const char * const *keys,
                                      const size_t *key_length,
                                      size_t number_of_keys)
{
  for (uint32_t x= 0; x < number_of_keys; x++)
  {
    memcached_return_t rc;
    rc= memcached_validate_key_length(*(key_length + x), false);
    if (rc != MEMCACHED_SUCCESS)
      return rc;
 
    for (size_t y= 0; y < *(key_length + x); y++)
    {
      if ((isgraph(keys[x][y])) == 0)
        return MEMCACHED_BAD_KEY_PROVIDED;
    }
  }

  return MEMCACHED_SUCCESS;
}

