#include <ctype.h>
#include <strings.h>
#include <memcached.h>

memcached_server_st *parse_opt_servers(char *server_strings)
{
  char *string;
  unsigned int port;
  char *begin_ptr;
  char *end_ptr;
  memcached_server_st *servers= NULL;
  memcached_return rc;

  assert(server_strings);

  end_ptr= server_strings + strlen(server_strings);

  for (begin_ptr= server_strings, string= index(server_strings, ','); 
       begin_ptr != end_ptr; 
       string= index(begin_ptr, ','))
  {
    char buffer[HUGE_STRING_LEN];
    char *ptr;
    port= 0;

    memset(buffer, 0, HUGE_STRING_LEN);
    if (string)
    {
      memcpy(buffer, begin_ptr, string - begin_ptr);
      begin_ptr= string+1;
    }
    else
    {
      size_t length= strlen(begin_ptr);
      memcpy(buffer, begin_ptr, length);
      begin_ptr= end_ptr;
    }

    ptr= index(buffer, ':');

    if (ptr)
    {
      ptr[0]= 0;

      ptr++;

      port= strtol(ptr, (char **)NULL, 10);
    }

    servers= memcached_server_list_append(servers, buffer, port, &rc);

    if (isspace(*begin_ptr))
      begin_ptr++;
  }

  return servers;
}

long int timedif(struct timeval a, struct timeval b)
{
  register int us, s;

  us = a.tv_usec - b.tv_usec;
  us /= 1000;
  s = a.tv_sec - b.tv_sec;
  s *= 1000;
  return s + us;
}
