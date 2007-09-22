#include <memcached.h>

static char **cleanup_list= NULL;
static char cleanup_list_length= 0;

void parse_opt_servers(memcached_st *memc,
                       char *server_strings)
{
  char *string;
  unsigned int port;
  char *begin_ptr;
  char *end_ptr;

  end_ptr= server_strings + strlen(server_strings);

  assert(server_strings);
  assert(memc);

  for (begin_ptr= server_strings, string= index(server_strings, ','); 
       begin_ptr == end_ptr; 
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
      size_t length= strlen(server_strings);
      memcpy(buffer, begin_ptr, length);
      string= server_strings+length;
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

void cleanup(void)
{
  unsigned int x;
  for (x= 0; x < cleanup_list_length; x++)
    free(cleanup_list[x]);

  free(cleanup_list);
}

char *strdup_cleanup(const char *str)
{
  char *ptr;

  ptr= strdup(str);

  cleanup_list= (char **)realloc(cleanup_list, sizeof(char *) * (cleanup_list_length+1));
  cleanup_list[cleanup_list_length]= ptr;
  cleanup_list_length++;

  return ptr;
}
