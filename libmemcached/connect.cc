/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Libmemcached library
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2006-2010 Brian Aker All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <libmemcached/common.h>
#include <cassert>
#include <ctime>
#include <sys/time.h>

static memcached_return_t connect_poll(memcached_server_st *ptr)
{
  struct pollfd fds[1];
  fds[0].fd = ptr->fd;
  fds[0].events = POLLOUT;

  size_t loop_max= 5;

  if (ptr->root->poll_timeout == 0)
  {
    return memcached_set_error(*ptr, MEMCACHED_TIMEOUT, MEMCACHED_AT);
  }

  while (--loop_max) // Should only loop on cases of ERESTART or EINTR
  {
    int error= poll(fds, 1, ptr->root->connect_timeout);
    switch (error)
    {
    case 1:
      {
        int err;
        socklen_t len= sizeof (err);
        (void)getsockopt(ptr->fd, SOL_SOCKET, SO_ERROR, &err, &len);

        // We check the value to see what happened wth the socket.
        if (err == 0)
        {
          return MEMCACHED_SUCCESS;
        }

        return memcached_set_errno(*ptr, err, MEMCACHED_AT);
      }
    case 0:
      {
        return memcached_set_error(*ptr, MEMCACHED_TIMEOUT, MEMCACHED_AT);
      }

    default: // A real error occurred and we need to completely bail
      WATCHPOINT_ERRNO(get_socket_errno());
      switch (get_socket_errno())
      {
#ifdef TARGET_OS_LINUX
      case ERESTART:
#endif
      case EINTR:
        continue;

      case EFAULT:
      case ENOMEM:
        return memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT);

      case EINVAL:
        return memcached_set_error(*ptr, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT, memcached_literal_param("RLIMIT_NOFILE exceeded, or if OSX the timeout value was invalid"));

      default: // This should not happen
        if (fds[0].revents & POLLERR)
        {
          int err;
          socklen_t len= sizeof (err);
          (void)getsockopt(ptr->fd, SOL_SOCKET, SO_ERROR, &err, &len);
          ptr->cached_errno= (err == 0) ? get_socket_errno() : err;
        }
        else
        {
          ptr->cached_errno= get_socket_errno();
        }

        WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
        (void)closesocket(ptr->fd);
        ptr->fd= INVALID_SOCKET;
        ptr->state= MEMCACHED_SERVER_STATE_NEW;

        return memcached_set_errno(*ptr, get_socket_errno(), MEMCACHED_AT);
      }
    }
  }

  // This should only be possible from ERESTART or EINTR;
  return memcached_set_errno(*ptr, get_socket_errno(), MEMCACHED_AT);
}

static memcached_return_t set_hostinfo(memcached_server_st *server)
{
  WATCHPOINT_ASSERT(not server->address_info); // We cover the case where a programming mistake has been made.
  if (server->address_info)
  {
    freeaddrinfo(server->address_info);
    server->address_info= NULL;
    server->address_info_next= NULL;
  }

  char str_port[NI_MAXSERV];
  int length= snprintf(str_port, NI_MAXSERV, "%u", (uint32_t)server->port);
  if (length >= NI_MAXSERV || length < 0)
    return MEMCACHED_FAILURE;

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));

#if 0
  hints.ai_family= AF_INET;
#endif
  if (server->type == MEMCACHED_CONNECTION_UDP)
  {
    hints.ai_protocol= IPPROTO_UDP;
    hints.ai_socktype= SOCK_DGRAM;
  }
  else
  {
    hints.ai_socktype= SOCK_STREAM;
    hints.ai_protocol= IPPROTO_TCP;
  }

  int errcode;
  switch(errcode= getaddrinfo(server->hostname, str_port, &hints, &server->address_info))
  {
  case 0:
    break;

  case EAI_AGAIN:
    return memcached_set_error(*server, MEMCACHED_TIMEOUT, MEMCACHED_AT, memcached_string_make_from_cstr(gai_strerror(errcode)));

  case EAI_SYSTEM:
      return memcached_set_errno(*server, errno, MEMCACHED_AT, memcached_literal_param("getaddrinfo(EAI_SYSTEM)"));

  case EAI_BADFLAGS:
    return memcached_set_error(*server, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT, memcached_literal_param("getaddrinfo(EAI_BADFLAGS)"));

  case EAI_MEMORY:
    return memcached_set_error(*server, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT, memcached_literal_param("getaddrinfo(EAI_MEMORY)"));

  default:
    {
      WATCHPOINT_STRING(server->hostname);
      WATCHPOINT_STRING(gai_strerror(errcode));
      return memcached_set_error(*server, MEMCACHED_HOST_LOOKUP_FAILURE, MEMCACHED_AT, memcached_string_make_from_cstr(gai_strerror(errcode)));
    }
  }
  server->address_info_next= server->address_info;
  server->state= MEMCACHED_SERVER_STATE_ADDRINFO;

  return MEMCACHED_SUCCESS;
}

static inline void set_socket_nonblocking(memcached_server_st *ptr)
{
#ifdef WIN32
  u_long arg = 1;
  if (ioctlsocket(ptr->fd, FIONBIO, &arg) == SOCKET_ERROR)
  {
    memcached_set_errno(*ptr, get_socket_errno(), NULL);
  }
#else
  int flags;

  do
  {
    flags= fcntl(ptr->fd, F_GETFL, 0);
  } while (flags == -1 && (errno == EINTR || errno == EAGAIN));

  if (flags == -1)
  {
    memcached_set_errno(*ptr, errno, NULL);
  }
  else if ((flags & O_NONBLOCK) == 0)
  {
    int rval;

    do
    {
      rval= fcntl(ptr->fd, F_SETFL, flags | O_NONBLOCK);
    } while (rval == -1 && (errno == EINTR || errno == EAGAIN));

    unlikely (rval == -1)
    {
      memcached_set_errno(*ptr, errno, NULL);
    }
  }
#endif
}

static void set_socket_options(memcached_server_st *ptr)
{
  WATCHPOINT_ASSERT(ptr->fd != -1);

  if (ptr->type == MEMCACHED_CONNECTION_UDP)
    return;

#ifdef HAVE_SNDTIMEO
  if (ptr->root->snd_timeout)
  {
    int error;
    struct timeval waittime;

    waittime.tv_sec= 0;
    waittime.tv_usec= ptr->root->snd_timeout;

    error= setsockopt(ptr->fd, SOL_SOCKET, SO_SNDTIMEO,
                      &waittime, (socklen_t)sizeof(struct timeval));
    WATCHPOINT_ASSERT(error == 0);
  }
#endif

#ifdef HAVE_RCVTIMEO
  if (ptr->root->rcv_timeout)
  {
    int error;
    struct timeval waittime;

    waittime.tv_sec= 0;
    waittime.tv_usec= ptr->root->rcv_timeout;

    error= setsockopt(ptr->fd, SOL_SOCKET, SO_RCVTIMEO,
                      &waittime, (socklen_t)sizeof(struct timeval));
    WATCHPOINT_ASSERT(error == 0);
  }
#endif


#if defined(__MACH__) && defined(__APPLE__) || defined(__FreeBSD__)
  {
    int set= 1;
    int error= setsockopt(ptr->fd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));

    // This is not considered a fatal error
    if (error == -1)
    {
      WATCHPOINT_ERRNO(get_socket_errno());
      perror("setsockopt(SO_NOSIGPIPE)");
    }
  }
#endif

  if (ptr->root->flags.no_block)
  {
    int error;
    struct linger linger;

    linger.l_onoff= 1;
    linger.l_linger= 0; /* By default on close() just drop the socket */
    error= setsockopt(ptr->fd, SOL_SOCKET, SO_LINGER,
                      &linger, (socklen_t)sizeof(struct linger));
    WATCHPOINT_ASSERT(error == 0);
  }

  if (ptr->root->flags.tcp_nodelay)
  {
    int flag= 1;
    int error;

    error= setsockopt(ptr->fd, IPPROTO_TCP, TCP_NODELAY,
                      &flag, (socklen_t)sizeof(int));
    WATCHPOINT_ASSERT(error == 0);
  }

  if (ptr->root->flags.tcp_keepalive)
  {
    int flag= 1;
    int error;

    error= setsockopt(ptr->fd, SOL_SOCKET, SO_KEEPALIVE,
                      &flag, (socklen_t)sizeof(int));
    WATCHPOINT_ASSERT(error == 0);
  }

#ifdef TCP_KEEPIDLE
  if (ptr->root->tcp_keepidle > 0)
  {
    int error;

    error= setsockopt(ptr->fd, IPPROTO_TCP, TCP_KEEPIDLE,
                      &ptr->root->tcp_keepidle, (socklen_t)sizeof(int));
    WATCHPOINT_ASSERT(error == 0);
  }
#endif

  if (ptr->root->send_size > 0)
  {
    int error;

    error= setsockopt(ptr->fd, SOL_SOCKET, SO_SNDBUF,
                      &ptr->root->send_size, (socklen_t)sizeof(int));
    WATCHPOINT_ASSERT(error == 0);
  }

  if (ptr->root->recv_size > 0)
  {
    int error;

    error= setsockopt(ptr->fd, SOL_SOCKET, SO_RCVBUF,
                      &ptr->root->recv_size, (socklen_t)sizeof(int));
    WATCHPOINT_ASSERT(error == 0);
  }


  /* libmemcached will always use nonblocking IO to avoid write deadlocks */
  set_socket_nonblocking(ptr);
}

static memcached_return_t unix_socket_connect(memcached_server_st *ptr)
{
#ifndef WIN32
  WATCHPOINT_ASSERT(ptr->fd == -1);

  if ((ptr->fd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
  {
    return memcached_set_errno(*ptr, errno, NULL);
  }

  struct sockaddr_un servAddr;

  memset(&servAddr, 0, sizeof (struct sockaddr_un));
  servAddr.sun_family= AF_UNIX;
  strncpy(servAddr.sun_path, ptr->hostname, sizeof(servAddr.sun_path)); /* Copy filename */

  do {
    if (connect(ptr->fd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
      switch (errno)
      {
      case EINPROGRESS:
      case EALREADY:
      case EINTR:
        continue;

      case EISCONN: /* We were spinning waiting on connect */
        {
          WATCHPOINT_ASSERT(0); // Programmer error
          break;
        }

      default:
        WATCHPOINT_ERRNO(errno);
        return memcached_set_errno(*ptr, errno, MEMCACHED_AT);
      }
    }
  } while (0);
  ptr->state= MEMCACHED_SERVER_STATE_CONNECTED;

  WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);

  return MEMCACHED_SUCCESS;
#else
  (void)ptr;
  return MEMCACHED_NOT_SUPPORTED;
#endif
}

static memcached_return_t network_connect(memcached_server_st *ptr)
{
  bool timeout_error_occured= false;

  WATCHPOINT_ASSERT(ptr->fd == INVALID_SOCKET);
  WATCHPOINT_ASSERT(ptr->cursor_active == 0);

  if (not ptr->address_info)
  {
    WATCHPOINT_ASSERT(ptr->state == MEMCACHED_SERVER_STATE_NEW);
    memcached_return_t rc;
    uint32_t counter= 5;
    while (--counter)
    {
      if ((rc= set_hostinfo(ptr)) != MEMCACHED_TIMEOUT)
        break;

#ifndef WIN32
      struct timespec dream, rem;

      dream.tv_nsec= 1000;
      dream.tv_sec= 0;

      nanosleep(&dream, &rem);
#endif
    }

    if (memcached_failed(rc))
      return rc;
  }

  /* Create the socket */
  while (ptr->address_info_next && ptr->fd == INVALID_SOCKET)
  {
    /* Memcache server does not support IPV6 in udp mode, so skip if not ipv4 */
    if (ptr->type == MEMCACHED_CONNECTION_UDP && ptr->address_info_next->ai_family != AF_INET)
    {
      ptr->address_info_next= ptr->address_info_next->ai_next;
      continue;
    }

    if ((ptr->fd= socket(ptr->address_info_next->ai_family,
                         ptr->address_info_next->ai_socktype,
                         ptr->address_info_next->ai_protocol)) < 0)
    {
      return memcached_set_errno(*ptr, get_socket_errno(), NULL);
    }

    set_socket_options(ptr);

    /* connect to server */
    if ((connect(ptr->fd, ptr->address_info_next->ai_addr, ptr->address_info_next->ai_addrlen) != SOCKET_ERROR))
    {
      ptr->state= MEMCACHED_SERVER_STATE_CONNECTED;
      break; // Success
    }

    /* An error occurred */
    switch (get_socket_errno())
    {
    case ETIMEDOUT:
      timeout_error_occured= true;
      break;

    case EWOULDBLOCK:
    case EINPROGRESS: // nonblocking mode - first return
    case EALREADY: // nonblocking mode - subsequent returns
      {
        ptr->state= MEMCACHED_SERVER_STATE_IN_PROGRESS;
        memcached_return_t rc= connect_poll(ptr);

        if (memcached_success(rc))
        {
          ptr->state= MEMCACHED_SERVER_STATE_CONNECTED;
          return MEMCACHED_SUCCESS;
        }

        // A timeout here is treated as an error, we will not retry
        if (rc == MEMCACHED_TIMEOUT)
        {
          timeout_error_occured= true;
        }
      }
      break;

    case EISCONN: // we are connected :-)
      WATCHPOINT_ASSERT(0); // This is a programmer's error
      break;

    case EINTR: // Special case, we retry ai_addr
      WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
      (void)closesocket(ptr->fd);
      ptr->fd= INVALID_SOCKET;
      continue;

    default:
      break;
    }

    WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
    (void)closesocket(ptr->fd);
    ptr->fd= INVALID_SOCKET;
    ptr->address_info_next= ptr->address_info_next->ai_next;
  }

  WATCHPOINT_ASSERT(ptr->fd == INVALID_SOCKET);

  if (timeout_error_occured)
  {
    if (ptr->fd != INVALID_SOCKET)
    {
      (void)closesocket(ptr->fd);
      ptr->fd= INVALID_SOCKET;
    }
  }

  WATCHPOINT_STRING("Never got a good file descriptor");
  /* Failed to connect. schedule next retry */
  if (ptr->root->retry_timeout)
  {
    struct timeval next_time;

    if (gettimeofday(&next_time, NULL) == 0)
      ptr->next_retry= next_time.tv_sec + ptr->root->retry_timeout;
  }

  if (timeout_error_occured)
    return memcached_set_error(*ptr, MEMCACHED_TIMEOUT, MEMCACHED_AT);

  return memcached_set_error(*ptr, MEMCACHED_CONNECTION_FAILURE, MEMCACHED_AT); /* The last error should be from connect() */
}

void set_last_disconnected_host(memcached_server_write_instance_st self)
{
  // const_cast
  memcached_st *root= (memcached_st *)self->root;

#if 0
  WATCHPOINT_STRING(self->hostname);
  WATCHPOINT_NUMBER(self->port);
  WATCHPOINT_ERRNO(self->cached_errno);
#endif
  memcached_server_free(root->last_disconnected_server);
  root->last_disconnected_server= memcached_server_clone(NULL, self);
}

memcached_return_t memcached_connect(memcached_server_write_instance_st ptr)
{
  memcached_return_t rc= MEMCACHED_NO_SERVERS;

  if (ptr->fd != INVALID_SOCKET)
    return MEMCACHED_SUCCESS;

  LIBMEMCACHED_MEMCACHED_CONNECT_START();

  /* both retry_timeout and server_failure_limit must be set in order to delay retrying a server on error. */
  WATCHPOINT_ASSERT(ptr->root);
  if (ptr->root->retry_timeout && ptr->next_retry)
  {
    struct timeval curr_time;

    gettimeofday(&curr_time, NULL);

    // We should optimize this to remove the allocation if the server was
    // the last server to die
    if (ptr->next_retry > curr_time.tv_sec)
    {
      set_last_disconnected_host(ptr);

      return memcached_set_error(*ptr, MEMCACHED_SERVER_MARKED_DEAD, MEMCACHED_AT);
    }
  }

  // If we are over the counter failure, we just fail. Reject host only
  // works if you have a set number of failures.
  if (ptr->root->server_failure_limit && ptr->server_failure_counter >= ptr->root->server_failure_limit)
  {
    set_last_disconnected_host(ptr);

    // @todo fix this by fixing behavior to no longer make use of
    // memcached_st
    if (_is_auto_eject_host(ptr->root))
    {
      run_distribution((memcached_st *)ptr->root);
    }

    return memcached_set_error(*ptr, MEMCACHED_SERVER_MARKED_DEAD, MEMCACHED_AT);
  }

  /* We need to clean up the multi startup piece */
  switch (ptr->type)
  {
  case MEMCACHED_CONNECTION_UNKNOWN:
    WATCHPOINT_ASSERT(0);
    rc= MEMCACHED_NOT_SUPPORTED;
    break;

  case MEMCACHED_CONNECTION_UDP:
  case MEMCACHED_CONNECTION_TCP:
    rc= network_connect(ptr);
#ifdef LIBMEMCACHED_WITH_SASL_SUPPORT
    if (ptr->fd != INVALID_SOCKET and ptr->root->sasl.callbacks)
    {
      rc= memcached_sasl_authenticate_connection(ptr);
      if (memcached_failed(rc) and ptr->fd != INVALID_SOCKET)
      {
        WATCHPOINT_ASSERT(ptr->fd != INVALID_SOCKET);
        (void)closesocket(ptr->fd);
        ptr->fd= INVALID_SOCKET;
      }
    }
#endif
    break;

  case MEMCACHED_CONNECTION_UNIX_SOCKET:
    rc= unix_socket_connect(ptr);
    break;

  case MEMCACHED_CONNECTION_MAX:
  default:
    WATCHPOINT_ASSERT(0);
  }

  if (memcached_success(rc))
  {
    ptr->server_failure_counter= 0;
    ptr->next_retry= 0;
  }
  else
  {
    memcached_set_error(*ptr, rc, MEMCACHED_AT);
    ptr->server_failure_counter++;
    set_last_disconnected_host(ptr);
  }

  LIBMEMCACHED_MEMCACHED_CONNECT_END();

  return rc;
}
