#include <memcached.h>

memcached_return memcached_server_add(memcached_st *ptr, char *hostname, unsigned int port)
{
  memcached_host_st *new_host_list;
  char *new_hostname;

  if (!port)
    port= MEMCACHED_DEFAULT_PORT; 

  if (!hostname)
    hostname= "localhost"; 


  new_host_list= (memcached_host_st *)realloc(ptr->hosts, sizeof(memcached_host_st) * (ptr->number_of_hosts+1));
  memset((new_host_list + (sizeof(memcached_host_st) * ptr->number_of_hosts)) - sizeof(memcached_host_st), 
         0, sizeof(memcached_host_st));
  
  if (!new_host_list)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  ptr->hosts= new_host_list;

  new_hostname=
    (char *)malloc(sizeof(char) * (strlen(hostname)+1));
  if (!new_hostname)
    return MEMCACHED_MEMORY_ALLOCATION_FAILURE;

  memset(new_hostname, 0, strlen(hostname)+1);
  memcpy(new_hostname, hostname, strlen(hostname));
  ptr->hosts[ptr->number_of_hosts].hostname= new_hostname;
  ptr->hosts[ptr->number_of_hosts].port= port;
  ptr->hosts[ptr->number_of_hosts].fd= -1;
  ptr->number_of_hosts++;

  return MEMCACHED_SUCCESS;
}

memcached_return memcached_connect(memcached_st *ptr)
{
  unsigned int x;
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;

  if (ptr->connected)
    return MEMCACHED_SUCCESS;

  if (!ptr->hosts)
  {
    memcached_return rc;
    rc= memcached_server_add(ptr, NULL, 0);

    if (rc != MEMCACHED_SUCCESS)
      return rc;
  }


  for (x= 0; x < ptr->number_of_hosts; x++)
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
  }

  ptr->connected= 1;

  return MEMCACHED_SUCCESS;
}
