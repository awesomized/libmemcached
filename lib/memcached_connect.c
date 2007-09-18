#include <memcached.h>

memcached_return memcached_server_add(memcached_st *ptr, char *hostname, unsigned int port)
{
  memcached_host_st *host_ptr, *prev_ptr;

  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  if (!hostname)
    hostname= "localhost"; 

  if (ptr->hosts)
  {
    for (host_ptr= ptr->hosts; host_ptr; host_ptr= host_ptr->next)
      prev_ptr= host_ptr;
    host_ptr= (memcached_host_st *)malloc(sizeof(memcached_host_st));
    if (!host_ptr)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
    prev_ptr->next= host_ptr;
  }
  else
  {
    ptr->hosts=
      host_ptr= (memcached_host_st *)malloc(sizeof(memcached_host_st));
    if (!host_ptr)
      return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
  }

  memset(host_ptr, 0, sizeof(memcached_host_st));
  host_ptr->hostname= (char *)malloc(sizeof(char) * strlen(hostname));

  if (!host_ptr->hostname)
  {
    free(host_ptr);
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;
  }
  memcpy(host_ptr->hostname, hostname, strlen(hostname));
  host_ptr->port= port;

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_connect(memcached_st *ptr)
{
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;
  memcached_host_st *host_ptr;

  if (ptr->connected)
    return MEMCACHED_SUCCESS;

  if (!ptr->hosts)
  {
    memcached_return rc;
    rc= memcached_server_add(ptr, NULL, 0);

    if (rc != MEMCACHED_SUCCESS)
      return rc;
  }


  for (host_ptr= ptr->hosts; host_ptr; host_ptr= host_ptr->next)
  {
    if ((h= gethostbyname(host_ptr->hostname)) == NULL)
    {
      fprintf(stderr, "unknown host '%s'\n", host_ptr->hostname);
      return MEMCACHED_HOST_LOCKUP_FAILURE;
    }

    servAddr.sin_family= h->h_addrtype;
    memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(host_ptr->port);

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
      fprintf(stderr, "cannot bind port TCP %u\n", host_ptr->port);
      return(MEMCACHED_CONNECTION_BIND_FAILURE);
    }

    /* connect to server */
    if (connect(ptr->fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    {
      fprintf(stderr, "cannot connect to host '%s' (%u)  (error: %s)\n", host_ptr->hostname, 
              host_ptr->port,
              strerror(errno));
      return MEMCACHED_HOST_LOCKUP_FAILURE;
    }
  }

  ptr->connected= 1;

  return MEMCACHED_SUCCESS;
}
