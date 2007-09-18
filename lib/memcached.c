/*
  Memcached library
*/
#include <memcached.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>


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

static memcached_return memcached_connect(memcached_st *ptr)
{
  int rc;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  char *server_name= "localhost";

  if (ptr->connected)
    return MEMCACHED_SUCCESS;


  if ((h= gethostbyname(server_name)) == NULL)
  {
    fprintf(stderr, "unknown host '%s'\n", server_name);
    return MEMCACHED_HOST_LOCKUP_FAILURE;
  }

  servAddr.sin_family= h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons(MEMCACHED_DEFAULT_PORT);

  /* Create the socket */
  if ((ptr->fd= socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    fprintf(stderr, "cannot open socket");
    return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;
  }


  /* bind any port number */
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);

  if (bind(ptr->fd, (struct sockaddr *) &localAddr, sizeof(localAddr)) < 0)
  {
    fprintf(stderr, "cannot bind port TCP %u\n", MEMCACHED_DEFAULT_PORT);
    return(MEMCACHED_CONNECTION_BIND_FAILURE);
  }

  /* connect to server */
  if (connect(ptr->fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
  {
    fprintf(stderr, "cannot connect to host '%s'\n", server_name);
    return MEMCACHED_HOST_LOCKUP_FAILURE;
  }

  ptr->connected= 1;

  return MEMCACHED_SUCCESS;
}

static memcached_return memcached_response(memcached_st *ptr, 
                                           char *buffer, size_t buffer_length)
{
  size_t send_length;
  send_length= read(ptr->fd, buffer, buffer_length);

  /* This should never happen, if it does it means that key must now be quite large. */
  assert(send_length != buffer_length);

  if (send_length)
    switch(buffer[0])
    {
    case 'V': /* VALUE */
      return MEMCACHED_SUCCESS;
    case 'O': /* OK */
      return MEMCACHED_SUCCESS;
    case 'S': /* STORED */
      {
        if (buffer[1] == 'T')
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
    case 'E': /* PROTOCOL ERROR */
      return MEMCACHED_PROTOCOL_ERROR;
    case 'C': /* CLIENT ERROR */
      return MEMCACHED_CLIENT_ERROR;
    default:
      return MEMCACHED_UNKNOWN_READ_FAILURE;
    }

  return MEMCACHED_READ_FAILURE;
}

static memcached_return memcached_send(memcached_st *ptr, 
                                       char *key, size_t key_length, 
                                       char *value, size_t value_length, 
                                       time_t expiration,
                                       uint16_t  flags,
                                       char *verb)
{
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%s %.*s %u %u %u\r\n", verb,
                        key_length, key, flags, expiration, value_length);
  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed set on %.*s TCP\n", key_length+1, key);

    return MEMCACHED_WRITE_FAILURE;
  }

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "%.*s\r\n", 
                        value_length, value);
  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed set on %.*s TCP\n", key_length+1, key);

    return MEMCACHED_WRITE_FAILURE;
  }
  
  send_length= read(ptr->fd, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);

  if (send_length && buffer[0] == 'S')  /* STORED */
    return MEMCACHED_SUCCESS;
  else if (send_length && buffer[0] == 'N')  /* NOT_STORED */
    return MEMCACHED_NOTSTORED;
  else
    return MEMCACHED_READ_FAILURE;
}

memcached_return memcached_set(memcached_st *ptr, char *key, size_t key_length, 
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags)
{
  return  memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, "set");
}

memcached_return memcached_add(memcached_st *ptr, char *key, size_t key_length,
                               char *value, size_t value_length, 
                               time_t expiration,
                               uint16_t  flags)
{
  return  memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, "add");
}

memcached_return memcached_replace(memcached_st *ptr, char *key, size_t key_length,
                                   char *value, size_t value_length, 
                                   time_t expiration,
                                   uint16_t  flags)
{
  return  memcached_send(ptr, key, key_length, value, value_length,
                         expiration, flags, "replace");
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

memcached_return memcached_stat(memcached_st *ptr, memcached_stat_st *stat)
{
  size_t send_length;
  memcached_return rc;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];

  rc= memcached_connect(ptr);

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, 
                        "stats \r\n");
  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed on stats\n");

    return MEMCACHED_WRITE_FAILURE;
  }

  while((send_length= read(ptr->fd, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE)) > 0)
  {
    if (send_length && buffer[0] == 'D') /* DELETED */
      return MEMCACHED_SUCCESS;
    else if (send_length && buffer[0] == 'N') /* NOT FOUND */
      return MEMCACHED_NOTFOUND;
    else if (send_length && buffer[0] == 'E') /* PROTOCOL ERROR */
      return MEMCACHED_PROTOCOL_ERROR;
    else if (send_length && buffer[0] == 'C') /* CLIENT ERROR */
      return MEMCACHED_CLIENT_ERROR;
    else if (send_length && buffer[0] == 'S') /* SERVER ERROR */
      return MEMCACHED_SERVER_ERROR;
    else if (send_length) /* UNKNOWN READ FAILURE */
    {
      fprintf(stderr, "UNKNOWN %.*s\n", send_length, buffer);
      return MEMCACHED_UNKNOWN_READ_FAILURE;
    }
    else
      return MEMCACHED_READ_FAILURE;
  }
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


char *memcached_get(memcached_st *ptr, char *key, size_t key_length, 
                    size_t *value_length, 
                    uint16_t *flags,
                    memcached_return *error)
{
  size_t send_length;
  char buffer[MEMCACHED_DEFAULT_COMMAND_SIZE];
  char *string_ptr;

  *error= memcached_connect(ptr);

  send_length= snprintf(buffer, MEMCACHED_DEFAULT_COMMAND_SIZE, "get %.*s\r\n", 
                        key_length, key);
  if (*error != MEMCACHED_SUCCESS)
    return NULL;

  if ((send(ptr->fd, buffer, send_length, 0) == -1))
  {
    fprintf(stderr, "failed fetch on %.*s TCP\n", key_length+1, key);
    *error= MEMCACHED_WRITE_FAILURE;
    return NULL;
  }

  memset(buffer, 0, MEMCACHED_DEFAULT_COMMAND_SIZE);
  *error= memcached_response(ptr, buffer, MEMCACHED_DEFAULT_COMMAND_SIZE);

  if (*error == MEMCACHED_SUCCESS)
  {
    char *end_ptr;

    string_ptr= buffer;
    string_ptr+= 6; /* "VALUE " */

    /* We do nothing with the key, since we only asked for one key */
    for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);

    /* Flags fetch */
    string_ptr= end_ptr + 1;
    for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);
    *flags= (uint16_t)strtol(string_ptr, &end_ptr, 10);

    /* Length fetch */
    string_ptr= end_ptr + 1;
    for (end_ptr= string_ptr; *end_ptr != ' '; end_ptr++);
    *value_length= strtoll(string_ptr, &end_ptr, 10);

    /* Skip past the \r\n */
    string_ptr= end_ptr +2;

    if (*value_length)
    {
      size_t need_to_copy;
      char *pos_ptr;
      char *value;

      value= (char *)malloc(*value_length * sizeof(char));

      if (!value)
      {
        *error= MEMCACHED_MEMORY_ALLOCATION_FAILURE;
        return NULL;
      }

      need_to_copy= (*value_length < (size_t)(buffer-string_ptr)) 
        ? *value_length 
        : (size_t)(buffer-string_ptr) ;

      pos_ptr= memcpy(value, string_ptr, need_to_copy);

      if ( need_to_copy > *value_length)
      {
        size_t read_length;
        size_t need_to_read;

        need_to_read= *value_length - need_to_copy;

        read_length= read(ptr->fd, pos_ptr, need_to_read);
        if (read_length != need_to_read)
        {
          free(value);
          *error= MEMCACHED_PARTIAL_READ;

          return NULL;
        }
      }

      return value;
    }
  }

  return NULL;
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
