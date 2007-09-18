/*
  Memcached library
*/
#include <memcached.h>

memcached_st *memcached_init(memcached_st *ptr)
{
  if (!ptr)
  {
    ptr= (memcached_st *)malloc(sizeof(memcached_st));

    if (!ptr)
      return NULL; /*  MEMCACHED_MEMORY_ALLOCATION_FAILURE */

    memset(ptr, 0, sizeof(memcached_st));
    ptr->is_allocated= MEMCACHED_ALLOCATED;
  }
  else
  {
    memset(ptr, 0, sizeof(memcached_st));
  }
  ptr->fd= -1;

  return ptr;
}

void memcached_server_add(memcached_st *ptr, char *server_name, unsigned int port)
{
}


memcached_return memcached_delete(memcached_st *ptr, char *key, size_t key_length,
                                  time_t expiration)
{
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  if (expiration)
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s %u\r\n", key_length, key, expiration);
  else
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "delete %.*s\r\n", key_length, key);
  if ((write(ptr->fd, buffer, send_length) == -1))
  {
    fprintf(stderr, "failed set on %.*s TCP\n", key_length+1, key);

    return MEMCACHED_WRITE_FAILURE;
  }

  return memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);
}

memcached_return memcached_increment(memcached_st *ptr, char *key, size_t key_length,
                                     unsigned int count)
{
  return MEMCACHED_SUCCESS;
}

memcached_return memcached_decrement(memcached_st *ptr, char *key, size_t key_length,
                                     unsigned int count)
{
  return MEMCACHED_SUCCESS;
}

memcached_return memcached_flush(memcached_st *ptr, time_t expiration)
{
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  if (expiration)
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "flush_all %u\r\n", expiration);
  else
    send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                          "flush_all\r\n");
  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed flush_all TCP\n");

    return MEMCACHED_WRITE_FAILURE;
  }

  return memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);
}

char *memcached_version(memcached_st *ptr, memcached_return *error)
{
  return MEMCACHED_SUCCESS;
}

memcached_return memcached_verbosity(memcached_st *ptr, unsigned int verbosity)
{
  return MEMCACHED_SUCCESS;
}

memcached_return memcached_quit(memcached_st *ptr)
{
  return MEMCACHED_SUCCESS;
}

void memcached_deinit(memcached_st *ptr)
{
  if (ptr->fd == -1)
    close(ptr->fd);

  if (ptr->is_allocated == MEMCACHED_ALLOCATED)
    free(ptr);
  else
    memset(ptr, 0, sizeof(memcached_st));
}

char *memcached_strerror(memcached_st *ptr, memcached_return rc)
{
  switch (rc)
  {
  case MEMCACHED_SUCCESS:
    return "SUCCESS";
  case MEMCACHED_FAILURE:
    return "FAILURE";
  case MEMCACHED_HOST_LOCKUP_FAILURE:
    return "HOSTNAME LOOKUP FAILURE";
  case MEMCACHED_CONNECTION_FAILURE:
    return "CONNECTION FAILURE";
  case MEMCACHED_CONNECTION_BIND_FAILURE:
    return "CONNECTION BIND FAILURE";
  case MEMCACHED_READ_FAILURE:
    return "READ FAILURE";
  case MEMCACHED_UNKNOWN_READ_FAILURE:
    return "UNKNOWN READ FAILURE";
  case MEMCACHED_PROTOCOL_ERROR:
    return "PROTOCOL ERROR";
  case MEMCACHED_CLIENT_ERROR:
    return "CLIENT ERROR";
  case MEMCACHED_SERVER_ERROR:
    return "SERVER ERROR";
  case MEMCACHED_WRITE_FAILURE:
    return "WRITE FAILURE";
  case MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE:
    return "CONNECTION SOCKET CREATE FAILURE";
  case MEMCACHED_DATA_EXISTS:
    return "CONNECTION DATA EXISTS";
  case MEMCACHED_DATA_DOES_NOT_EXIST:
    return "CONNECTION DATA DOES NOT EXIST";
  case MEMCACHED_NOTSTORED:
    return "NOT STORED";
  case MEMCACHED_NOTFOUND:
    return "NOT FOUND";
  case MEMCACHED_MEMORY_ALLOCATION_FAILURE:
    return "MEMORY ALLOCATION FAILURE";
  case MEMCACHED_PARTIAL_READ:
    return "PARTIAL READ";
  };
}
