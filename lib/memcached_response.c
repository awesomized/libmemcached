/*
  Memcached library

  memcached_response() is used to determine the return result
  from an issued command.
*/

#include <memcached.h>

memcached_return memcached_response(memcached_st *ptr, 
                                    char *buffer, size_t buffer_length,
                                    unsigned int server_key)
{
  size_t send_length;

  memset(buffer, 0, buffer_length);
  send_length= read(ptr->hosts[server_key].fd, buffer, buffer_length);

  if (send_length)
    switch(buffer[0])
    {
    case 'V': /* VALUE */
      return MEMCACHED_SUCCESS;
    case 'O': /* OK */
      return MEMCACHED_SUCCESS;
    case 'S': /* STORED STATS SERVER_ERROR */
      {
        if (buffer[1] == 'T') /* STORED STATS */
          return MEMCACHED_SUCCESS;
        else if (buffer[1] == 'E')
          return MEMCACHED_SERVER_ERROR;
        else
          return MEMCACHED_UNKNOWN_READ_FAILURE;
      }
    case 'D': /* DELETED */
      return MEMCACHED_SUCCESS;
    case 'N': /* NOT_FOUND */
      {
        if (buffer[4] == 'F')
          return MEMCACHED_NOTFOUND;
        else if (buffer[4] == 'S')
          return MEMCACHED_NOTSTORED;
        else
          return MEMCACHED_UNKNOWN_READ_FAILURE;
      }
    case 'E': /* PROTOCOL ERROR or END */
      {
        if (buffer[1] == 'N')
          return MEMCACHED_NOTFOUND;
        else if (buffer[1] == 'R')
          return MEMCACHED_PROTOCOL_ERROR;
        else
          return MEMCACHED_UNKNOWN_READ_FAILURE;
      }
    case 'C': /* CLIENT ERROR */
      return MEMCACHED_CLIENT_ERROR;
    default:
      return MEMCACHED_UNKNOWN_READ_FAILURE;
    }

  return MEMCACHED_READ_FAILURE;
}
