/* 
  I debated about putting this in the client library since it does an 
  action I don't really believe belongs in the library.

  Frankly its too damn useful not to be here though.
*/

#include <memcached.h>
#include "common.h"

memcached_server_st *memcached_servers_parse(char *server_strings)
{
  char *string;
  unsigned int port;
  char *begin_ptr;
  char *end_ptr;
  memcached_server_st *servers= NULL;
  memcached_return rc;

  WATCHPOINT_ASSERT(server_strings);

  end_ptr= server_strings + strlen(server_strings);

  for (begin_ptr= server_strings, string= index(server_strings, ','); 
       begin_ptr != end_ptr; 
       string= index(begin_ptr, ','))
  {
    char buffer[HUGE_STRING_LEN];
    char *ptr;
    port= 0;

    if (string)
    {
      memcpy(buffer, begin_ptr, string - begin_ptr);
      buffer[(unsigned int)(string - begin_ptr)]= 0;
      begin_ptr= string+1;
    }
    else
    {
      size_t length= strlen(begin_ptr);
      memcpy(buffer, begin_ptr, length);
      buffer[length]= 0;
      begin_ptr= end_ptr;
    }

    ptr= index(buffer, ':');

    if (ptr)
    {
      ptr[0]= 0;

      ptr++;

      port= strtoul(ptr, (char **)NULL, 10);
    }

    servers= memcached_server_list_append(servers, buffer, port, &rc);

    if (isspace(*begin_ptr))
      begin_ptr++;
  }

  return servers;
}
