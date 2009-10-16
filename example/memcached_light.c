/* -*- Mode: C; tab-width: 2; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/**
 * What is a library without an example to show you how to use the library?
 * This example use both interfaces to implement a small memcached server.
 * Please note that this is an exemple on how to use the library, not
 * an implementation of a scalable memcached server. If you look closely
 * at the example it isn't even multithreaded ;-)
 *
 * With that in mind, let me give you some pointers into the source:
 *   storage.c/h       - Implements the item store for this server and not really
 *                       interesting for this example.
 *   interface_v0.c    - Shows an implementation of the memcached server by using
 *                       the "raw" access to the packets as they arrive
 *   interface_v1.c    - Shows an implementation of the memcached server by using
 *                       the more "logical" interface.
 *   memcached_light.c - This file sets up all of the sockets and run the main
 *                       message loop.
 *
 *
 * config.h is included so that I can use the ntohll/htonll on platforms that
 * doesn't have that (this is a private function inside libmemcached, so you
 * cannot use it directly from libmemcached without special modifications to
 * the library)
 */

#include "config.h"
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#include <libmemcached/protocol_handler.h>
#include <libmemcached/byteorder.h>
#include "storage.h"
#include "memcached_light.h"

extern memcached_binary_protocol_callback_st interface_v0_impl;
extern memcached_binary_protocol_callback_st interface_v1_impl;

static int server_sockets[1024];
static int num_server_sockets= 0;
static void* socket_userdata_map[1024];
static bool verbose= false;

/**
 * Create a socket and bind it to a specific port number
 * @param port the port number to bind to
 */
static int server_socket(const char *port) {
  struct addrinfo *ai;
  struct addrinfo hints= { .ai_flags= AI_PASSIVE,
                           .ai_family= AF_UNSPEC,
                           .ai_socktype= SOCK_STREAM };

  int error= getaddrinfo("127.0.0.1", port, &hints, &ai);
  if (error != 0)
  {
    if (error != EAI_SYSTEM)
      fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(error));
    else
      perror("getaddrinfo()");

    return 1;
  }

  struct linger ling= {0, 0};

  for (struct addrinfo *next= ai; next; next= next->ai_next)
  {
    int sock= socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sock == -1)
    {
      perror("Failed to create socket");
      continue;
    }

    int flags= fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
      perror("Failed to get socket flags");
      close(sock);
      continue;
    }

    if ((flags & O_NONBLOCK) != O_NONBLOCK)
    {
      if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
      {
        perror("Failed to set socket to nonblocking mode");
        close(sock);
        continue;
      }
    }

    flags= 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags)) != 0)
      perror("Failed to set SO_REUSEADDR");

    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags)) != 0)
      perror("Failed to set SO_KEEPALIVE");

    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling)) != 0)
      perror("Failed to set SO_LINGER");

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags)) != 0)
      perror("Failed to set TCP_NODELAY");

    if (bind(sock, next->ai_addr, next->ai_addrlen) == -1)
    {
      if (errno != EADDRINUSE)
      {
        perror("bind()");
        freeaddrinfo(ai);
      }
      close(sock);
      continue;
    }

    if (listen(sock, 1024) == -1)
    {
      perror("listen()");
      close(sock);
      continue;
    }

    server_sockets[num_server_sockets++]= sock;
  }

  freeaddrinfo(ai);

  return (num_server_sockets > 0) ? 0 : 1;
}

/**
 * Convert a command code to a textual string
 * @param cmd the comcode to convert
 * @return a textual string with the command or NULL for unknown commands
 */
static const char* comcode2str(uint8_t cmd)
{
  static const char * const text[] = {
    "GET", "SET", "ADD", "REPLACE", "DELETE",
    "INCREMENT", "DECREMENT", "QUIT", "FLUSH",
    "GETQ", "NOOP", "VERSION", "GETK", "GETKQ",
    "APPEND", "PREPEND", "STAT", "SETQ", "ADDQ",
    "REPLACEQ", "DELETEQ", "INCREMENTQ", "DECREMENTQ",
    "QUITQ", "FLUSHQ", "APPENDQ", "PREPENDQ"
  };

  if (cmd <= PROTOCOL_BINARY_CMD_PREPENDQ)
    return text[cmd];

  return NULL;
}

/**
 * Print out the command we are about to execute
 */
static void pre_execute(const void *cookie __attribute__((unused)),
                        protocol_binary_request_header *header __attribute__((unused)))
{
  if (verbose)
  {
    const char *cmd= comcode2str(header->request.opcode);
    if (cmd != NULL)
      fprintf(stderr, "pre_execute from %p: %s\n", cookie, cmd);
    else
      fprintf(stderr, "pre_execute from %p: 0x%02x\n", cookie, header->request.opcode);
  }
}

/**
 * Print out the command we just executed
 */
static void post_execute(const void *cookie __attribute__((unused)),
                         protocol_binary_request_header *header __attribute__((unused)))
{
  if (verbose)
  {
    const char *cmd= comcode2str(header->request.opcode);
    if (cmd != NULL)
      fprintf(stderr, "post_execute from %p: %s\n", cookie, cmd);
    else
      fprintf(stderr, "post_execute from %p: 0x%02x\n", cookie, header->request.opcode);
  }
}

/**
 * Callback handler for all unknown commands.
 * Send an unknown command back to the client
 */
static protocol_binary_response_status unknown(const void *cookie,
                                               protocol_binary_request_header *header,
                                               memcached_binary_protocol_raw_response_handler response_handler)
{
  protocol_binary_response_no_extras response= {
    .message= {
      .header.response= {
        .magic= PROTOCOL_BINARY_RES,
        .opcode= header->request.opcode,
        .status= htons(PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND),
        .opaque= header->request.opaque
      }
    }
  };

  return response_handler(cookie, header, (void*)&response);
}

static void work(void);

/**
 * Program entry point. Bind to the specified port(s) and serve clients
 *
 * @param argc number of items in the argument vector
 * @param argv argument vector
 * @return 0 on success, 1 otherwise
 */
int main(int argc, char **argv)
{
  bool port_specified= false;
  int cmd;
  memcached_binary_protocol_callback_st *interface= &interface_v0_impl;

  /*
   * We need to initialize the handlers manually due to a bug in the
   * warnings generated by struct initialization in gcc (all the way up to 4.4)
   */
  initialize_interface_v0_handler();

  while ((cmd= getopt(argc, argv, "v1p:?")) != EOF)
  {
    switch (cmd) {
    case '1':
      interface= &interface_v1_impl;
      break;
    case 'p':
      port_specified= true;
      (void)server_socket(optarg);
      break;
    case 'v':
      verbose= true;
      break;
    case '?':  /* FALLTHROUGH */
    default:
      (void)fprintf(stderr, "Usage: %s [-p port] [-v] [-1]\n", argv[0]);
      return 1;
    }
  }

  if (!initialize_storage())
  {
    /* Error message already printed */
    return 1;
  }

  if (!port_specified)
    (void)server_socket("9999");

  if (num_server_sockets == 0)
  {
    fprintf(stderr, "I don't have any server sockets\n");
    return 1;
  }

  /*
   * Create and initialize the handles to the protocol handlers. I want
   * to be able to trace the traffic throught the pre/post handlers, and
   * set up a common handler for unknown messages
   */
  interface->pre_execute= pre_execute;
  interface->post_execute= post_execute;
  interface->unknown= unknown;

  struct memcached_protocol_st *protocol_handle;
  if ((protocol_handle= memcached_protocol_create_instance()) == NULL)
  {
    fprintf(stderr, "Failed to allocate protocol handle\n");
    return 1;
  }

  memcached_binary_protocol_set_callbacks(protocol_handle, interface);
  memcached_binary_protocol_set_pedantic(protocol_handle, true);

  for (int xx= 0; xx < num_server_sockets; ++xx)
    socket_userdata_map[server_sockets[xx]]= protocol_handle;

  /* Serve all of the clients */
  work();

  /* NOTREACHED */
  return 0;
}

static void work(void)
{
#define MAX_SERVERS_TO_POLL 100
  struct pollfd fds[MAX_SERVERS_TO_POLL];
  int max_poll;

  for (max_poll= 0; max_poll < num_server_sockets; ++max_poll)
  {
    fds[max_poll].events= POLLIN;
    fds[max_poll].revents= 0;
    fds[max_poll].fd= server_sockets[max_poll];
    ++max_poll;
  }

  while (true)
  {
    int err= poll(fds, (nfds_t)max_poll, -1);

    if (err == 0 || (err == -1 && errno != EINTR))
    {
      perror("poll() failed");
      abort();
    }

    /* find the available filedescriptors */
    for (int x= max_poll - 1; x > -1 && err > 0; --x)
    {
      if (fds[x].revents != 0)
      {
        --err;
        if (x < num_server_sockets)
        {
          /* accept new client */
          struct sockaddr_storage addr;
          socklen_t addrlen= sizeof(addr);
          int sock= accept(fds[x].fd, (struct sockaddr *)&addr,
                           &addrlen);

          if (sock == -1)
          {
            perror("Failed to accept client");
            continue;
          }

          struct memcached_protocol_st *protocol;
          protocol= socket_userdata_map[fds[x].fd];

          struct memcached_protocol_client_st* c;
          c= memcached_protocol_create_client(protocol, sock);
          if (c == NULL)
          {
            fprintf(stderr, "Failed to create client\n");
            close(sock);
          }
          else
          {
            socket_userdata_map[sock]= c;
            fds[max_poll].events= POLLIN;
            fds[max_poll].revents= 0;
            fds[max_poll].fd= sock;
            ++max_poll;
          }
        }
        else
        {
          /* drive the client */
          struct memcached_protocol_client_st* c;
          c= socket_userdata_map[fds[x].fd];
          assert(c != NULL);
          fds[max_poll].events= 0;

          memcached_protocol_event_t events= memcached_protocol_client_work(c);
          if (events & MEMCACHED_PROTOCOL_WRITE_EVENT)
            fds[max_poll].events= POLLOUT;

          if (events & MEMCACHED_PROTOCOL_READ_EVENT)
            fds[max_poll].events= POLLIN;

          if (!(events & MEMCACHED_PROTOCOL_PAUSE_EVENT ||
                fds[max_poll].events != 0))
          {
            memcached_protocol_client_destroy(c);
            close(fds[x].fd);
            fds[x].events= 0;

            if (x != max_poll - 1)
              memmove(fds + x, fds + x + 1, (size_t)(max_poll - x));

            --max_poll;
          }
        }
      }
    }
  }
}
