#include "common.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#include <netdb.h>

static memcached_return set_hostinfo(memcached_server_st *server)
{
  struct hostent *h;

  if ((h= gethostbyname(server->hostname)) == NULL)
  {
    return MEMCACHED_HOST_LOCKUP_FAILURE;
  }

  server->servAddr.sin_family= h->h_addrtype;
  memcpy((char *) &server->servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  server->servAddr.sin_port = htons(server->port);

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

    if (ptr->hosts[server_key].servAddr.sin_family == 0)
    {
      memcached_return rc;

      rc= set_hostinfo(&ptr->hosts[server_key]);
      if (rc != MEMCACHED_SUCCESS)
        return rc;
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

    if (ptr->hosts[server_key].servAddr.sin_family == 0)
    {
      memcached_return rc;

      rc= set_hostinfo(&ptr->hosts[server_key]);
      if (rc != MEMCACHED_SUCCESS)
        return rc;
    }

    /* Create the socket */
    if ((ptr->hosts[server_key].fd= socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      ptr->cached_errno= errno;
      return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;
    }

    /* For the moment, not getting a nonblocking mode will note be fatal */
    if (ptr->flags & MEM_NO_BLOCK)
    {
      int flags;

      flags= fcntl(ptr->hosts[server_key].fd, F_GETFL, 0);
      if (flags != -1)
        (void)fcntl(ptr->hosts[server_key].fd, F_SETFL, flags | O_NONBLOCK);
    }

    if (ptr->flags & MEM_TCP_NODELAY)
    {
      int flag= 1;

      setsockopt(ptr->hosts[server_key].fd, IPPROTO_TCP, TCP_NODELAY, 
                 &flag, (socklen_t)sizeof(int));
    }

    if (ptr->send_size)
    {
      setsockopt(ptr->hosts[server_key].fd, SOL_SOCKET, SO_SNDBUF, 
                 &ptr->send_size, (socklen_t)sizeof(int));
    }

    if (ptr->recv_size)
    {
      setsockopt(ptr->hosts[server_key].fd, SOL_SOCKET, SO_SNDBUF, 
                 &ptr->recv_size, (socklen_t)sizeof(int));
    }

    /* connect to server */
test_connect:
    if (connect(ptr->hosts[server_key].fd, 
                (struct sockaddr *)&ptr->hosts[server_key].servAddr, 
                sizeof(struct sockaddr)) < 0)
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
  if (server_key)
  {
    rc= tcp_connect(ptr, server_key);
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
  }
  else
  {
    unsigned int x;

    for (x= 0; x < ptr->number_of_hosts; x++)
    {
      memcached_return possible_rc;

      possible_rc= MEMCACHED_NOT_SUPPORTED; /* Remove warning */

      switch (ptr->hosts[x].type)
      {
      case MEMCACHED_CONNECTION_UNKNOWN:
        WATCHPOINT_ASSERT(0);
        possible_rc= MEMCACHED_NOT_SUPPORTED;
        break;
      case MEMCACHED_CONNECTION_UDP:
        possible_rc= udp_connect(ptr, x);
        break;
      case MEMCACHED_CONNECTION_TCP:
        possible_rc= tcp_connect(ptr, x);
        break;
      case MEMCACHED_CONNECTION_UNIX_SOCKET:
        possible_rc= unix_socket_connect(ptr, x);
        break;
      }
      rc= MEMCACHED_SUCCESS;

      if (possible_rc != MEMCACHED_SUCCESS)
        rc= MEMCACHED_SOME_ERRORS;
    }
  }
  LIBMEMCACHED_MEMCACHED_CONNECT_END();

  return rc;
}
