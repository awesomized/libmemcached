/*
  Memcached library

  memcached_response() is used to determine the return result
  from an issued command.
*/

#include "common.h"
#include "memcached_io.h"

memcached_return memcached_response(memcached_st *ptr, 
                                    char *buffer, size_t buffer_length,
                                    memcached_result_st *result,
                                    unsigned int server_key)
{
  unsigned int x;
  size_t send_length;
  char *buffer_ptr;
  unsigned int max_messages;


  send_length= 0;

  /* We may have old commands in the buffer not set, first purge */
  if (ptr->flags & MEM_NO_BLOCK)
    (void)memcached_io_write(ptr, server_key, NULL, 0, 1);

  max_messages= memcached_server_response_count(ptr, server_key);
  for (x= 0; x <  max_messages; x++)
  {
    size_t total_length= 0;
    buffer_ptr= buffer;

    while (1)
    {
      unsigned int read_length;

      read_length= memcached_io_read(ptr, server_key,
                                     buffer_ptr, 1);

      if (read_length != 1)
        return  MEMCACHED_UNKNOWN_READ_FAILURE;

      if (*buffer_ptr == '\n')
        break;
      else
        buffer_ptr++;

      total_length++;
      WATCHPOINT_ASSERT(total_length <= buffer_length);

      if (total_length >= buffer_length)
        return MEMCACHED_PROTOCOL_ERROR;
    }
    buffer_ptr++;
    *buffer_ptr= 0;

    memcached_server_response_decrement(ptr, server_key);
  }

  switch(buffer[0])
  {
  case 'V': /* VALUE || VERSION */
    if (buffer[1] == 'A') /* VALUE */
    {
      memcached_return rc;

      /* We add back in one because we will need to search for END */
      memcached_server_response_increment(ptr, server_key);
      if (result)
        rc= value_fetch(ptr, buffer, result, server_key);
      else
        rc= value_fetch(ptr, buffer, &ptr->result, server_key);

      return rc;
    }
    else if (buffer[1] == 'E') /* VERSION */
    {
      return MEMCACHED_SUCCESS;
    }
    else
    {
      WATCHPOINT_STRING(buffer);
      WATCHPOINT_ASSERT(0);
      return MEMCACHED_UNKNOWN_READ_FAILURE;
    }
  case 'O': /* OK */
    return MEMCACHED_SUCCESS;
  case 'S': /* STORED STATS SERVER_ERROR */
    {
      if (buffer[2] == 'A') /* STORED STATS */
      {
        memcached_server_response_increment(ptr, server_key);
        return MEMCACHED_STAT;
      }
      else if (buffer[1] == 'E')
        return MEMCACHED_SERVER_ERROR;
      else if (buffer[1] == 'T')
        return MEMCACHED_STORED;
      else
      {
        WATCHPOINT_STRING(buffer);
        WATCHPOINT_ASSERT(0);
        return MEMCACHED_UNKNOWN_READ_FAILURE;
      }
    }
  case 'D': /* DELETED */
    return MEMCACHED_DELETED;
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
        return MEMCACHED_END;
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

  return MEMCACHED_SUCCESS;
}

char *memcached_result_value(memcached_result_st *ptr)
{
  memcached_string_st *sptr= &ptr->value;
  return memcached_string_value(sptr);
}

size_t memcached_result_length(memcached_result_st *ptr)
{
  memcached_string_st *sptr= &ptr->value;
  return memcached_string_length(sptr);
}
