#include "common.h"

memcached_return memcached_do(memcached_st *ptr, unsigned int server_key, char *command, 
                              size_t command_length, char with_flush)
{
  memcached_return rc;
  ssize_t sent_length;

  WATCHPOINT_ASSERT(command_length);
  WATCHPOINT_ASSERT(command);

  if (ptr->hosts[server_key].cursor_active)
    (void)memcached_finish_server(ptr, server_key);

  if ((rc= memcached_connect(ptr, server_key)) != MEMCACHED_SUCCESS)
    return rc;

  sent_length= memcached_io_write(ptr, server_key, command, command_length, with_flush);

  if (sent_length == -1 || sent_length != command_length)
    rc= MEMCACHED_WRITE_FAILURE;

  return rc;
}
