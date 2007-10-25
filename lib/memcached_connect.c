#include "common.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>

memcached_return memcached_real_connect(memcached_st *ptr, unsigned int server_key)
{
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;

  if (ptr->hosts[server_key].fd == -1)
  {
    /* Old connection junk still is in the structure */
    assert(ptr->hosts[server_key].stack_responses == 0);

    if ((h= gethostbyname(ptr->hosts[server_key].hostname)) == NULL)
    {
      ptr->my_errno= h_errno;
      return MEMCACHED_HOST_LOCKUP_FAILURE;
    }

    servAddr.sin_family= h->h_addrtype;
    memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    servAddr.sin_port = htons(ptr->hosts[server_key].port);

    /* Create the socket */
    if ((ptr->hosts[server_key].fd= socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      ptr->my_errno= errno;
      return MEMCACHED_CONNECTION_SOCKET_CREATE_FAILURE;
    }

    /* bind any port number */
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(0);

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
    if (connect(ptr->hosts[server_key].fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
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
        ptr->my_errno= errno;
        return MEMCACHED_ERRNO;
      }
      ptr->connected++;
    }
    assert(ptr->hosts[server_key].stack_responses == 0);
  }

  return MEMCACHED_SUCCESS;
}


memcached_return memcached_connect(memcached_st *ptr, unsigned int server_key)
{
  memcached_return rc= MEMCACHED_NO_SERVERS;
  LIBMEMCACHED_MEMCACHED_CONNECT_START();

  if (ptr->connected == ptr->number_of_hosts)
    return MEMCACHED_SUCCESS;

  if (!ptr->hosts)
    return MEMCACHED_NO_SERVERS;

  /* We need to clean up the multi startup piece */
  if (server_key)
    rc= memcached_real_connect(ptr, server_key);
  else
  {
    unsigned int x;

    for (x= 0; x < ptr->number_of_hosts; x++)
    {
      memcached_return possible_rc;

      possible_rc= memcached_real_connect(ptr, x);
      rc= MEMCACHED_SUCCESS;

      if (possible_rc != MEMCACHED_SUCCESS)
        rc= MEMCACHED_SOME_ERRORS;
    }
  }
  LIBMEMCACHED_MEMCACHED_CONNECT_END();

  return rc;
}
