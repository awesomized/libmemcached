/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "libmemcached/common.h"
#include "p9y/poll.hpp"

#include <cassert>

static memcached_return_t set_hostinfo(memcached_instance_st *server) {
  assert(server->type != MEMCACHED_CONNECTION_UNIX_SOCKET);
  assert(server->hostname());

  server->clear_addrinfo();

  char str_host[MEMCACHED_NI_MAXHOST] = {0}, str_port[MEMCACHED_NI_MAXSERV] = {0};
  errno = 0;

  auto length = snprintf(str_port, MEMCACHED_NI_MAXSERV, "%u", uint32_t(server->port()));
  if (length <= 0 or errno) {
    return memcached_set_error(*server, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT,
                               memcached_literal_param("snprintf(NI_MAXSERV)"));
  }

  struct addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  if (memcached_is_udp(server->root)) {
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_socktype = SOCK_DGRAM;
  } else {
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;
  }

  auto hostname = server->hostname();
  if (*hostname == '[') {
    auto closing_bracket = &hostname[strlen(hostname) - 1];
    if (*closing_bracket == ']') {
      auto host_len = closing_bracket - hostname - 1;
      if (host_len < MEMCACHED_NI_MAXHOST) {
        hostname = strncpy(str_host, hostname + 1, host_len);
      }
    }
  }

  auto errcode = getaddrinfo(hostname, str_port, &hints, &server->address_info);
  switch (errcode) {
  case 0:
    server->address_info_next = server->address_info;
    server->state = MEMCACHED_SERVER_STATE_ADDRINFO;
    break;

  case EAI_AGAIN:
    return memcached_set_error(*server, MEMCACHED_TIMEOUT, MEMCACHED_AT,
                               memcached_string_make_from_cstr(gai_strerror(errcode)));

  case EAI_SYSTEM:
    server->clear_addrinfo();
    return memcached_set_errno(*server, errno, MEMCACHED_AT,
                               memcached_literal_param("getaddrinfo(EAI_SYSTEM)"));

  case EAI_BADFLAGS:
    server->clear_addrinfo();
    return memcached_set_error(*server, MEMCACHED_INVALID_ARGUMENTS, MEMCACHED_AT,
                               memcached_literal_param("getaddrinfo(EAI_BADFLAGS)"));

  case EAI_MEMORY:
    server->clear_addrinfo();
    return memcached_set_error(*server, MEMCACHED_MEMORY_ALLOCATION_FAILURE, MEMCACHED_AT,
                               memcached_literal_param("getaddrinfo(EAI_MEMORY)"));

  default: {
    server->clear_addrinfo();
    return memcached_set_error(*server, MEMCACHED_HOST_LOOKUP_FAILURE, MEMCACHED_AT,
                               memcached_string_make_from_cstr(gai_strerror(errcode)));
  }
  }

  return MEMCACHED_SUCCESS;
}

static inline void set_socket_nonblocking(memcached_instance_st *server) {
#if defined(_WIN32)
  u_long arg = 1;
  if (ioctlsocket(server->fd, FIONBIO, &arg) == SOCKET_ERROR) {
    memcached_set_errno(*server, get_socket_errno(), NULL);
  }
#else
  int flags;

  if (SOCK_NONBLOCK == 0) {
    do {
      flags = fcntl(server->fd, F_GETFL, 0);
    } while (flags == -1 && (errno == EINTR || errno == EAGAIN));

    if (flags == -1) {
      memcached_set_errno(*server, errno, NULL);
    } else if ((flags & O_NONBLOCK) == 0) {
      int rval;

      do {
        rval = fcntl(server->fd, F_SETFL, flags | O_NONBLOCK);
      } while (rval == -1 && (errno == EINTR or errno == EAGAIN));

      if (rval == -1) {
        memcached_set_errno(*server, errno, NULL);
      }
    }
  }
#endif
}

static bool set_socket_options(memcached_instance_st *server) {
  assert_msg(server->fd != INVALID_SOCKET, "invalid socket was passed to set_socket_options()");

#ifdef HAVE_FCNTL
  // If SOCK_CLOEXEC exists then we don't need to call the following
  if (SOCK_CLOEXEC == 0) {
    if (FD_CLOEXEC) {
      int flags;
      do {
        flags = fcntl(server->fd, F_GETFD, 0);
      } while (flags == -1 and (errno == EINTR or errno == EAGAIN));

      if (flags != -1) {
        int rval;
        do {
          rval = fcntl(server->fd, F_SETFD, flags | FD_CLOEXEC);
        } while (rval == -1 && (errno == EINTR or errno == EAGAIN));
        // we currently ignore the case where rval is -1
      }
    }
  }
#endif

  if (memcached_is_udp(server->root)) {
    return true;
  }

#ifdef HAVE_SO_SNDTIMEO
  if (server->root->snd_timeout > 0) {
    struct timeval waittime;

    waittime.tv_sec = server->root->snd_timeout / 1000000;
    waittime.tv_usec = server->root->snd_timeout % 1000000;

    int error = setsockopt(server->fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &waittime,
                           (socklen_t) sizeof(struct timeval));
    (void) error;
    assert(error == 0);
  }
#endif

#ifdef HAVE_SO_RCVTIMEO
  if (server->root->rcv_timeout > 0) {
    struct timeval waittime;

    waittime.tv_sec = server->root->rcv_timeout / 1000000;
    waittime.tv_usec = server->root->rcv_timeout % 1000000;

    int error = setsockopt(server->fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &waittime,
                           (socklen_t) sizeof(struct timeval));
    (void) (error);
    assert(error == 0);
  }
#endif

#if defined(_WIN32)
#else
#  if defined(SO_NOSIGPIPE)
  if (SO_NOSIGPIPE) {
    int set = 1;
    int error = setsockopt(server->fd, SOL_SOCKET, SO_NOSIGPIPE, (void *) &set, sizeof(int));

    assert(error == 0);

    // This is not considered a fatal error
    if (error == -1) {
#    if 0
      perror("setsockopt(SO_NOSIGPIPE)");
#    endif
    }
  }
#  endif // SO_NOSIGPIPE
#endif   // _WIN32

  if (server->root->flags.no_block) {
    struct linger linger;

    linger.l_onoff = 1;
    linger.l_linger = 0; /* By default on close() just drop the socket */
    int error = setsockopt(server->fd, SOL_SOCKET, SO_LINGER, (char *) &linger,
                           (socklen_t) sizeof(struct linger));
    (void) (error);
    assert(error == 0);
  }

  if (TCP_NODELAY) {
    if (server->root->flags.tcp_nodelay) {
      int flag = 1;

      int error =
          setsockopt(server->fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, (socklen_t) sizeof(int));
      (void) (error);
      assert(error == 0);
    }
  }

  if (server->root->flags.tcp_keepalive) {
    int flag = 1;

    int error =
        setsockopt(server->fd, SOL_SOCKET, SO_KEEPALIVE, (char *) &flag, (socklen_t) sizeof(int));
    (void) (error);
    assert(error == 0);
  }

  if (TCP_KEEPIDLE) {
    if (server->root->tcp_keepidle > 0) {
      int error = setsockopt(server->fd, IPPROTO_TCP, TCP_KEEPIDLE,
                             (char *) &server->root->tcp_keepidle, (socklen_t) sizeof(int));
      (void) (error);
      assert(error == 0);
    }
  }

  if (server->root->send_size > 0) {
    int error = setsockopt(server->fd, SOL_SOCKET, SO_SNDBUF, (char *) &server->root->send_size,
                           (socklen_t) sizeof(int));
    (void) (error);
    assert(error == 0);
  }

  if (server->root->recv_size > 0) {
    int error = setsockopt(server->fd, SOL_SOCKET, SO_RCVBUF, (char *) &server->root->recv_size,
                           (socklen_t) sizeof(int));
    (void) (error);
    assert(error == 0);
  }

  /* libmemcached will always use nonblocking IO to avoid write deadlocks */
  set_socket_nonblocking(server);

  return true;
}

static memcached_return_t unix_socket_connect(memcached_instance_st *server) {
#ifndef _WIN32
  WATCHPOINT_ASSERT(server->fd == INVALID_SOCKET);

  do {
    int type = SOCK_STREAM;
    if (SOCK_CLOEXEC) {
      type |= SOCK_CLOEXEC;
    }

    if (SOCK_NONBLOCK) {
      type |= SOCK_NONBLOCK;
    }

    if ((server->fd = socket(AF_UNIX, type, 0)) == -1) {
      return memcached_set_errno(*server, errno, NULL);
    }

    struct sockaddr_un servAddr;

    memset(&servAddr, 0, sizeof(struct sockaddr_un));
    servAddr.sun_family = AF_UNIX;
    if (strlen(server->hostname()) >= sizeof(servAddr.sun_path)) {
      return memcached_set_error(*server, MEMCACHED_FAIL_UNIX_SOCKET, MEMCACHED_AT);
    }
    strncpy(servAddr.sun_path, server->hostname(),
            sizeof(servAddr.sun_path) - 1); /* Copy filename */

    if (connect(server->fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
      switch (errno) {
      case EINPROGRESS:
      case EALREADY:
      case EAGAIN:
        server->events(POLLOUT);
        break;

      case EINTR:
        server->reset_socket();
        continue;

      case EISCONN: /* We were spinning waiting on connect */
      {
        assert(0); // Programmer error
        server->reset_socket();
        continue;
      }

      default:
        WATCHPOINT_ERRNO(errno);
        server->reset_socket();
        return memcached_set_errno(*server, errno, MEMCACHED_AT);
      }
    }
  } while (0);
  server->state = MEMCACHED_SERVER_STATE_CONNECTED;

  WATCHPOINT_ASSERT(server->fd != INVALID_SOCKET);

  return MEMCACHED_SUCCESS;
#else
  (void) server;
  return MEMCACHED_NOT_SUPPORTED;
#endif
}

static memcached_return_t network_connect(memcached_instance_st *server) {
  bool timeout_error_occured = false;

  WATCHPOINT_ASSERT(server->fd == INVALID_SOCKET);
  WATCHPOINT_ASSERT(server->cursor_active_ == 0);

  /*
    We want to check both of these because if address_info_next has been fully tried, we want to do
    a new lookup to make sure we have picked up on any new DNS information.
  */
  if (server->address_info == NULL or server->address_info_next == NULL) {
    WATCHPOINT_ASSERT(server->state == MEMCACHED_SERVER_STATE_NEW);
    server->address_info_next = NULL;
    memcached_return_t rc = set_hostinfo(server);

    if (memcached_failed(rc)) {
      return rc;
    }
  }

  assert(server->address_info_next);
  assert(server->address_info);

  /* Create the socket */
  while (server->address_info_next and server->fd == INVALID_SOCKET) {
    int type = server->address_info_next->ai_socktype;
    if (SOCK_CLOEXEC) {
      type |= SOCK_CLOEXEC;
    }

    if (SOCK_NONBLOCK) {
      type |= SOCK_NONBLOCK;
    }

    server->fd =
        socket(server->address_info_next->ai_family, type, server->address_info_next->ai_protocol);

    if (int(server->fd) == SOCKET_ERROR) {
      return memcached_set_errno(*server, get_socket_errno(), NULL);
    }

    if (set_socket_options(server) == false) {
      server->reset_socket();
      return MEMCACHED_CONNECTION_FAILURE;
    }

    /* connect to server */
    if ((connect(server->fd, server->address_info_next->ai_addr,
                 server->address_info_next->ai_addrlen)
         != SOCKET_ERROR))
    {
      server->state = MEMCACHED_SERVER_STATE_CONNECTED;
      return MEMCACHED_SUCCESS;
    }

    /* An error occurred */
    int local_error = get_socket_errno();
    switch (local_error) {
    case ETIMEDOUT:
      timeout_error_occured = true;
      break;

#if EWOULDBLOCK != EAGAIN
    case EWOULDBLOCK:
#endif
    case EAGAIN:
    case EINPROGRESS: // nonblocking mode - first return
    case EALREADY:    // nonblocking mode - subsequent returns
    {
      server->events(POLLOUT);
      server->state = MEMCACHED_SERVER_STATE_IN_PROGRESS;
      memcached_return_t rc = memcached_io_poll(server, IO_POLL_CONNECT, local_error);

      if (memcached_success(rc)) {
        server->state = MEMCACHED_SERVER_STATE_CONNECTED;
        return MEMCACHED_SUCCESS;
      }

      // A timeout here is treated as an error, we will not retry
      if (rc == MEMCACHED_TIMEOUT) {
        timeout_error_occured = true;
      }
    } break;

    case EISCONN:           // we are connected :-)
      WATCHPOINT_ASSERT(0); // This is a programmer's error
      break;

    case EINTR: // Special case, we retry ai_addr
      WATCHPOINT_ASSERT(server->fd != INVALID_SOCKET);
      server->reset_socket();
      continue;

    case ECONNREFUSED:
      // Probably not running service

    default:
      memcached_set_errno(*server, local_error, MEMCACHED_AT);
      break;
    }

    WATCHPOINT_ASSERT(server->fd != INVALID_SOCKET);
    server->reset_socket();
    server->address_info_next = server->address_info_next->ai_next;
  }

  WATCHPOINT_ASSERT(server->fd == INVALID_SOCKET);

  if (timeout_error_occured) {
    server->reset_socket();
  }

  WATCHPOINT_STRING("Never got a good file descriptor");

  if (memcached_has_current_error(*server)) {
    return memcached_instance_error_return(server);
  }

  if (timeout_error_occured and server->state < MEMCACHED_SERVER_STATE_IN_PROGRESS) {
    return memcached_set_error(
        *server, MEMCACHED_TIMEOUT, MEMCACHED_AT,
        memcached_literal_param(
            "if (timeout_error_occured and server->state < MEMCACHED_SERVER_STATE_IN_PROGRESS)"));
  }

  return memcached_set_error(*server, MEMCACHED_CONNECTION_FAILURE,
                             MEMCACHED_AT); /* The last error should be from connect() */
}

/*
  backoff_handling()

  Based on time/failure count fail the connect without trying. This prevents waiting in a state
  where we get caught spending cycles just waiting.
*/
static memcached_return_t backoff_handling(memcached_instance_st *server, bool &in_timeout) {
  struct timeval curr_time;
  bool _gettime_success = (gettimeofday(&curr_time, NULL) == 0);

  /*
    If we hit server_failure_limit then something is completely wrong about the server.

    1) If autoeject is enabled we do that.
    2) If not? We go into timeout again, there is much else to do :(
  */
  if (server->server_failure_counter >= server->root->server_failure_limit) {
    /*
      We just auto_eject if we hit this point
    */
    if (_is_auto_eject_host(server->root)) {
      set_last_disconnected_host(server);

      // Retry dead servers if requested
      if (_gettime_success and server->root->dead_timeout > 0) {
        server->next_retry = curr_time.tv_sec + server->root->dead_timeout;

        // We only retry dead servers once before assuming failure again
        server->server_failure_counter = server->root->server_failure_limit - 1;
      }

      memcached_return_t rc;
      if (memcached_failed(rc = run_distribution((memcached_st *) server->root))) {
        return memcached_set_error(
            *server, rc, MEMCACHED_AT,
            memcached_literal_param("Backoff handling failed during run_distribution"));
      }

      return memcached_set_error(*server, MEMCACHED_SERVER_MARKED_DEAD, MEMCACHED_AT);
    }

    server->state = MEMCACHED_SERVER_STATE_IN_TIMEOUT;

    // Sanity check/setting
    if (server->next_retry == 0) {
      server->next_retry = 1;
    }
  }

  if (server->state == MEMCACHED_SERVER_STATE_IN_TIMEOUT) {
    /*
      If next_retry is less then our current time, then we reset and try everything again.
    */
    if (_gettime_success and server->next_retry < curr_time.tv_sec) {
      server->state = MEMCACHED_SERVER_STATE_NEW;
      server->server_timeout_counter = 0;
    } else {
      return memcached_set_error(*server, MEMCACHED_SERVER_TEMPORARILY_DISABLED, MEMCACHED_AT);
    }

    in_timeout = true;
  }

  return MEMCACHED_SUCCESS;
}

static memcached_return_t _memcached_connect(memcached_instance_st *server,
                                             const bool set_last_disconnected) {
  assert(server);
  if (server->fd != INVALID_SOCKET) {
    return MEMCACHED_SUCCESS;
  }

  LIBMEMCACHED_MEMCACHED_CONNECT_START();

  bool in_timeout = false;
  memcached_return_t rc;
  if (memcached_failed(rc = backoff_handling(server, in_timeout))) {
    set_last_disconnected_host(server);
    return rc;
  }

  if (LIBMEMCACHED_WITH_SASL_SUPPORT and server->root->sasl.callbacks
      and memcached_is_udp(server->root))
  {
    return memcached_set_error(
        *server, MEMCACHED_INVALID_HOST_PROTOCOL, MEMCACHED_AT,
        memcached_literal_param("SASL is not supported for UDP connections"));
  }

  if (server->hostname()[0] == '/') {
    server->type = MEMCACHED_CONNECTION_UNIX_SOCKET;
  }

  /* We need to clean up the multi startup piece */
  switch (server->type) {
  case MEMCACHED_CONNECTION_UDP:
  case MEMCACHED_CONNECTION_TCP:
    rc = network_connect(server);

#if defined(LIBMEMCACHED_WITH_SASL_SUPPORT)
    if (LIBMEMCACHED_WITH_SASL_SUPPORT) {
      if (server->fd != INVALID_SOCKET and server->root->sasl.callbacks) {
        rc = memcached_sasl_authenticate_connection(server);
        if (memcached_failed(rc) and server->fd != INVALID_SOCKET) {
          WATCHPOINT_ASSERT(server->fd != INVALID_SOCKET);
          server->reset_socket();
        }
      }
    }
#endif
    break;

  case MEMCACHED_CONNECTION_UNIX_SOCKET:
    rc = unix_socket_connect(server);
    break;
  }

  if (memcached_success(rc)) {
    server->mark_server_as_clean();
    memcached_version_instance(server);
    return rc;
  } else if (set_last_disconnected) {
    set_last_disconnected_host(server);
    if (memcached_has_current_error(*server)) {
      memcached_mark_server_for_timeout(server);
      assert(memcached_failed(memcached_instance_error_return(server)));
    } else {
      memcached_set_error(*server, rc, MEMCACHED_AT);
      memcached_mark_server_for_timeout(server);
    }

    LIBMEMCACHED_MEMCACHED_CONNECT_END();

    if (in_timeout) {
      char buffer[1024];
      int snprintf_length =
          snprintf(buffer, sizeof(buffer), "%s:%d", server->hostname(), int(server->port()));
      return memcached_set_error(*server, MEMCACHED_SERVER_TEMPORARILY_DISABLED, MEMCACHED_AT,
                                 buffer, snprintf_length);
    }
  }

  return rc;
}

memcached_return_t memcached_connect(memcached_instance_st *server) {
  return _memcached_connect(server, true);
}
