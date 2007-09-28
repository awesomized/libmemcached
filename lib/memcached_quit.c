#include "common.h"

/*
  This closes all connections (forces flush of input as well).
  
  Maybe add a host specific, or key specific version? 
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
        close(ptr->hosts[x].fd);
        ptr->hosts[x].fd= -1;
      }
    }
  }

  ptr->connected= 0;
}
