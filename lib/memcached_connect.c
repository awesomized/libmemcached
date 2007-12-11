#include "common.h"

static memcached_return set_hostinfo(memcached_server_st *server)
{
  struct addrinfo *ai;
  struct addrinfo hints;
  int e;
  char str_port[NI_MAXSERV];

  sprintf(str_port, "%u", server->port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family= AF_INET;
  hints.ai_socktype= SOCK_STREAM;
  hints.ai_protocol= 0;

  e= getaddrinfo(server->hostname, str_port, &hints, &ai);
  if (e != 0)
  {
    WATCHPOINT_STRING(server->hostname);
    WATCHPOINT_STRING(gai_strerror(e));
    return MEMCACHED_HOST_LOOKUP_FAILURE;
  }

  if (server->address_info)
    freeaddrinfo(server->address_info);
  server->address_info= ai;

  return MEMCACHED_SUCCESS;
}

static memcached_return unix_socket_connect(memcached_st *ptr, unsigned int server_key)
{
  struct sockaddr_un servAddr;
  socklen_t addrlen;

  if (ptr->hosts[server_key].fd == -1)
  {
    if ((ptr->hosts[server_key].fd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      ptr->cached_errno= errno;
      return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;
    }

    memset(&servAddr, 0, sizeof (struct sockaddr_un));
    servAddr.sun_family= AF_UNIX;
    strcpy(servAddr.sun_path, ptr->hosts[server_key].hostname); /* Copy filename */

    addrlen= strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

test_connect:
    if (connect(ptr->hosts[server_key].fd, 
                (struct sockaddr *)&servAddr,
                sizeof(servAddr)) < 0)
    {
      switch (errno) {
        /* We are spinning waiting on connect */
      case EALREADY:
      case EINPROGRESS:
      case EINTR:
        goto test_connect;
      case EISCONN: /* We were spinning waiting on connect */
        break;
      default:
        WATCHPOINT_ERRNO(errno);
        ptr->cached_errno= errno;
        return MEMCACHED_ERRNO;
      }
      ptr->connected++;
    }
  }
  return MEMCACHED_SUCCESS;
}

static memcached_return udp_connect(memcached_st *ptr, unsigned int server_key)
{
  if (ptr->hosts[server_key].fd == -1)
  {
    /* Old connection junk still is in the structure */
    WATCHPOINT_ASSERT(ptr->hosts[server_key].stack_responses == 0);

    /*
      If we have not allocated the hosts object.
      Or if the cache has not been set.
    */
    if (ptr->hosts[server_key].sockaddr_inited == MEMCACHED_NOT_ALLOCATED || 
        (!(ptr->flags & MEM_USE_CACHE_LOOKUPS)))
    {
      memcached_return rc;

      rc= set_hostinfo(&ptr->hosts[server_key]);
      if (rc != MEMCACHED_SUCCESS)
        return rc;

      ptr->hosts[server_key].sockaddr_inited= MEMCACHED_ALLOCATED;
    }

    /* Create the socket */
    if ((ptr->hosts[server_key].fd= socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
      ptr->cached_errno= errno;
      return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;
    }
  }

  return MEMCACHED_SUCCESS;
}

static memcached_return tcp_connect(memcached_st *ptr, unsigned int server_key)
{
  if (ptr->hosts[server_key].fd == -1)
  {
    /* Old connection junk still is in the structure */
    WATCHPOINT_ASSERT(ptr->hosts[server_key].stack_responses == 0);
    struct addrinfo *use;

    if (ptr->hosts[server_key].sockaddr_inited == MEMCACHED_NOT_ALLOCATED || 
        (!(ptr->flags & MEM_USE_CACHE_LOOKUPS)))
    {
      memcached_return rc;

      rc= set_hostinfo(&ptr->hosts[server_key]);
      if (rc != MEMCACHED_SUCCESS)
        return rc;
      ptr->hosts[server_key].sockaddr_inited= MEMCACHED_ALLOCATED;
    }
    use= ptr->hosts[server_key].address_info;

    /* Create the socket */
    if ((ptr->hosts[server_key].fd= socket(use->ai_family, 
                                           use->ai_socktype, 
                                           use->ai_protocol)) < 0)
    {
      ptr->cached_errno= errno;
      WATCHPOINT_ERRNO(errno);
      return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;
    }

    if (ptr->flags & MEM_NO_BLOCK)
    {
      int error;
      struct linger linger;

      linger.l_onoff= 1; 
      linger.l_linger= MEMCACHED_DEFAULT_TIMEOUT; 
      error= setsockopt(ptr->hosts[server_key].fd, SOL_SOCKET, SO_LINGER, 
                        &linger, (socklen_t)sizeof(struct linger));
      WATCHPOINT_ASSERT(error == 0);
    }

    if (ptr->flags & MEM_TCP_NODELAY)
    {
      int flag= 1;
      int error;

      error= setsockopt(ptr->hosts[server_key].fd, IPPROTO_TCP, TCP_NODELAY, 
                        &flag, (socklen_t)sizeof(int));
      WATCHPOINT_ASSERT(error == 0);
    }

    if (ptr->send_size)
    {
      int error;

      error= setsockopt(ptr->hosts[server_key].fd, SOL_SOCKET, SO_SNDBUF, 
                        &ptr->send_size, (socklen_t)sizeof(int));
      WATCHPOINT_ASSERT(error == 0);
    }

    if (ptr->recv_size)
    {
      int error;

      error= setsockopt(ptr->hosts[server_key].fd, SOL_SOCKET, SO_SNDBUF, 
                        &ptr->recv_size, (socklen_t)sizeof(int));
      WATCHPOINT_ASSERT(error == 0);
    }

    /* For the moment, not getting a nonblocking mode will not be fatal */
    if (ptr->flags & MEM_NO_BLOCK)
    {
      int flags;

      flags= fcntl(ptr->hosts[server_key].fd, F_GETFL, 0);
      if (flags != -1)
      {
        (void)fcntl(ptr->hosts[server_key].fd, F_SETFL, flags | O_NONBLOCK);
      }
    }


    /* connect to server */
test_connect:
    if (connect(ptr->hosts[server_key].fd, 
                use->ai_addr, 
                use->ai_addrlen) < 0)
    {
      switch (errno) {
        /* We are spinning waiting on connect */
      case EALREADY:
      case EINPROGRESS:
      case EINTR:
        goto test_connect;
      case EISCONN: /* We were spinning waiting on connect */
        break;
      default:
        ptr->cached_errno= errno;
        WATCHPOINT_ERRNO(ptr->cached_errno);
        return MEMCACHED_ERRNO;
      }
      ptr->connected++;
    }

    WATCHPOINT_ASSERT(ptr->hosts[server_key].stack_responses == 0);
  }

  return MEMCACHED_SUCCESS;
}


memcached_return memcached_connect(memcached_st *ptr, unsigned int server_key)
{
  memcached_return rc= MEMCACHED_NO_SERVERS;
  LIBMEMCACHED_MEMCACHED_CONNECT_START();

  if (ptr->connected == ptr->number_of_hosts && ptr->number_of_hosts)
    return MEMCACHED_SUCCESS;

  if (ptr->hosts == NULL || ptr->number_of_hosts == 0)
    return MEMCACHED_NO_SERVERS;

  /* We need to clean up the multi startup piece */
  switch (ptr->hosts[server_key].type)
  {
  case MEMCACHED_CONNECTION_UNKNOWN:
    WATCHPOINT_ASSERT(0);
    rc= MEMCACHED_NOT_SUPPORTED;
    break;
  case MEMCACHED_CONNECTION_UDP:
    rc= udp_connect(ptr, server_key);
    break;
  case MEMCACHED_CONNECTION_TCP:
    rc= tcp_connect(ptr, server_key);
    break;
  case MEMCACHED_CONNECTION_UNIX_SOCKET:
    rc= unix_socket_connect(ptr, server_key);
    break;
  }

  if (rc != MEMCACHED_SUCCESS)
    WATCHPOINT_ERROR(rc);

  LIBMEMCACHED_MEMCACHED_CONNECT_END();

  return rc;
}
