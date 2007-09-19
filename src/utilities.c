#include <memcached.h>

void parse_opt_servers(memcached_st *memc,
                       char *opt_servers)
{
  char *string;
  unsigned int port;
  char *begin_ptr;

  for (begin_ptr= opt_servers, string= index(opt_servers, ','); 
       begin_ptr[0]; 
       begin_ptr= ++string, string= index(begin_ptr, ','))
  {
    char buffer[HUGE_STRING_LEN];
    char *ptr;

    memset(buffer, 0, HUGE_STRING_LEN);
    if (string)
    {
      memcpy(buffer, begin_ptr, string - begin_ptr);
    }
    else
    {
      size_t length= strlen(opt_servers);
      memcpy(buffer, begin_ptr, length);
      string= opt_servers+length;
    }

    ptr= index(buffer, ':');

    if (ptr)
    {
      ptr[0]= 0;

      ptr++;

      port= strtol(ptr, (char **)NULL, 10);

      memcached_server_add(memc, buffer, port);
    }
    else
    {
      memcached_server_add(memc, buffer, 0);
    }
  }
}

