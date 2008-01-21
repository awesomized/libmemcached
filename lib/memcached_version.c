#include "common.h"

const char * memcached_lib_version(void) 
{
  return LIBMEMCACHED_VERSION_STRING;
}

memcached_return memcached_version(memcached_st *ptr)
{
  unsigned int x;
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *response_ptr;
  char *command= "version\r\n";

  send_length= strlen(command);

  rc= MEMCACHED_SUCCESS;
  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    memcached_return rrc;

    rrc= memcached_do(&ptr->hosts[x], command, send_length, 1);
    if (rrc != MEMCACHED_SUCCESS)
    {
      rc= MEMCACHED_SOME_ERRORS;
      continue;
    }

    rrc= memcached_response(&ptr->hosts[x], buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, NULL);
    if (rrc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;

    /* Find the space, and then move one past it to copy version */
    response_ptr= index(buffer, ' ');
    response_ptr++;

    ptr->hosts[x].major_version= (uint8_t)strtol(response_ptr, (char **)NULL, 10);
    response_ptr= index(response_ptr, '.');
    response_ptr++;
    ptr->hosts[x].minor_version= (uint8_t)strtol(response_ptr, (char **)NULL, 10);
    response_ptr= index(response_ptr, '.');
    response_ptr++;
    ptr->hosts[x].micro_version= (uint8_t)strtol(response_ptr, (char **)NULL, 10);
  }

  return rc;
}
