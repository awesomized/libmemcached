#include "common.h"

/*
  This closes all connections (forces flush of input as well).
  
  Maybe add a host specific, or key specific version? 
  
  The reason we send "quit" is that in case we have buffered IO, this 
  will force data to be completed.
*/
void memcached_quit(memcached_st *ptr)
{
  unsigned int x;

  if (ptr->hosts)
  {
    for (x= 0; x < ptr->number_of_hosts; x++)
    {
      if (ptr->hosts[x].fd != -1)
      {
        char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

        if (ptr->flags & MEM_NO_BLOCK)
          memcached_io_write(ptr, x, "quit\r\n", 6, 1);
        close(ptr->hosts[x].fd);
        ptr->hosts[x].fd= -1;
      }
    }
  }

  ptr->connected= 0;
}
