#include <memcached.h>

memcached_return memcached_verbosity(memcached_st *ptr, unsigned int verbosity)
{
  unsigned int x;
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  if (rc != MEMCACHED_SUCCESS)
    rc= MEMCACHED_SOME_ERRORS;

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "verbosity %u\r\n", verbosity);

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    memcached_return rc;

    if ((write(ptr->hosts[x].fd, buffer, send_length) == -1))
    {
      fprintf(stderr, "failed verbosity\n");

      return MEMCACHED_WRITE_FAILURE;
    }

    rc= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, x);

    if (rc != MEMCACHED_SUCCESS)
      rc= MEMCACHED_SOME_ERRORS;
  }

  return rc;
}
