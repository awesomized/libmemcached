#include <memcached.h>

memcached_return memcached_connect(memcached_st *ptr)
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
