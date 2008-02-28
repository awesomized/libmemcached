#include "common.h"

memcached_return memcached_server_cursor(memcached_st *ptr, 
                                         memcached_server_function *callback,
                                         void *context,
                                         unsigned int number_of_callbacks)
{
  unsigned int y;

  for (y= 0; y < ptr->number_of_hosts; y++)
  {
    unsigned int x;

    for (x= 0; x < number_of_callbacks; x++)
    {
      unsigned int iferror;

      iferror= (*callback[x])(ptr, &ptr->hosts[y], context);

      if (iferror)
        continue;
    }
  }

  return MEMCACHED_SUCCESS;
}
