#include "common.h"

memcached_return memcached_connect(memcached_st *ptr)
{
  unsigned int x;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;

  LIBMEMCACHED_MEMCACHED_CONNECT_START();

  if (ptr->connected == ptr->number_of_hosts)
    return MEMCACHED_SUCCESS;

  if (!ptr->hosts)
    return MEMCACHED_NO_SERVERS;

  for (x= 0; x < ptr->number_of_hosts; x++)
  {
    if (ptr->hosts[x].fd == -1)
    {
      if ((h= gethostbyname(ptr->hosts[x].hostname)) == NULL)
        return MEMCACHED_HOST_LOCKUP_FAILURE;

      servAddr.sin_family= h->h_addrtype;
      memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
      servAddr.sin_port = htons(ptr->hosts[x].port);

      /* Create the socket */
      if ((ptr->hosts[0].fd= socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;


      /* bind any port number */
      localAddr.sin_family = AF_INET;
      localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      localAddr.sin_port = htons(0);

      if (bind(ptr->hosts[0].fd, (struct sockaddr *) &localAddr, sizeof(localAddr)) < 0)
        return(MEMCACHED_CONNECTION_BIND_FAILURE);

      /* connect to server */
      if (connect(ptr->hosts[0].fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        return MEMCACHED_HOST_LOCKUP_FAILURE;

      ptr->connected++;
    }
  }
  LIBMEMCACHED_MEMCACHED_CONNECT_END();

  return MEMCACHED_SUCCESS;
}
