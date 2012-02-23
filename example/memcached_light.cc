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

#include <libmemcachedprotocol-0.0/handler.h>
#include <libmemcached/socket.hpp>
#include <example/byteorder.h>
#include "example/storage.h"
#include "example/memcached_light.h"

#include <event.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <getopt.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

extern memcached_binary_protocol_callback_st interface_v0_impl;
extern memcached_binary_protocol_callback_st interface_v1_impl;

static memcached_socket_t server_sockets[1024];
static int num_server_sockets= 0;

struct connection
{
  void *userdata;
  struct event event;
};

/* The default maximum number of connections... (change with -c) */
static int maxconns= 1024;

static struct connection *socket_userdata_map;
static struct event_base *event_base;

struct options_st {
  char *pid_file;
  bool is_verbose;

  options_st() :
    pid_file(NULL),
    is_verbose(false)
  {
  }
};

static options_st global_options;

/**
 * Callback for driving a client connection
 * @param fd the socket for the client socket
 * @param which identifying the event that occurred (not used)
 * @param arg the connection structure for the client
 */
static void drive_client(memcached_socket_t fd, short, void *arg)
{
  struct connection *client= (struct connection*)arg;
  struct memcached_protocol_client_st* c= (struct memcached_protocol_client_st*)client->userdata;
  assert(c != NULL);

  memcached_protocol_event_t events= memcached_protocol_client_work(c);
  if (events & MEMCACHED_PROTOCOL_ERROR_EVENT)
  {
    if (global_options.is_verbose)
    {
      struct sockaddr_in sin;
      socklen_t addrlen= sizeof(sin);

      if (getsockname(fd, (struct sockaddr *)&sin, &addrlen) != -1)
      {
        std::cout << __FILE__ << ":" << __LINE__
          << " close(MEMCACHED_PROTOCOL_ERROR_EVENT)"
          << " " << inet_ntoa(sin.sin_addr) << ":" << sin.sin_port
          << " fd:" << fd
          << std::endl;
      }
      else
      {
        std::cout << __FILE__ << ":" << __LINE__ << "close() MEMCACHED_PROTOCOL_ERROR_EVENT" << std::endl;
      }
    }

    memcached_protocol_client_destroy(c);
    closesocket(fd);
  }
  else
  {
    short flags = 0;
    if (events & MEMCACHED_PROTOCOL_WRITE_EVENT)
    {
      flags= EV_WRITE;
    }

    if (events & MEMCACHED_PROTOCOL_READ_EVENT)
    {
      flags|= EV_READ;
    }

    event_set(&client->event, (intptr_t)fd, flags, drive_client, client);
    event_base_set(event_base, &client->event);

    if (event_add(&client->event, 0) == -1)
    {
      memcached_protocol_client_destroy(c);
      closesocket(fd);
    }
  }
}

/**
 * Callback for accepting new connections
 * @param fd the socket for the server socket
 * @param which identifying the event that occurred (not used)
 * @param arg the connection structure for the server
 */
static void accept_handler(memcached_socket_t fd, short, void *arg)
{
  struct connection *server= (struct connection *)arg;
  /* accept new client */
  struct sockaddr_storage addr;
  socklen_t addrlen= sizeof(addr);
  memcached_socket_t sock= accept(fd, (struct sockaddr *)&addr, &addrlen);

  if (sock == INVALID_SOCKET)
  {
    perror("Failed to accept client");
  }

#ifndef WIN32
  if (sock >= maxconns)
  {
    closesocket(sock);
    return ;
  }
#endif

  struct memcached_protocol_client_st* c= memcached_protocol_create_client((memcached_protocol_st*)server->userdata, sock);
  if (c == NULL)
  {
    closesocket(sock);
  }
  else
  {
    memcached_protocol_client_set_verbose(c, global_options.is_verbose);
    struct connection *client = &socket_userdata_map[sock];
    client->userdata= c;

    event_set(&client->event, (intptr_t)sock, EV_READ, drive_client, client);
    event_base_set(event_base, &client->event);
    if (event_add(&client->event, 0) == -1)
    {
      std::cerr << "Failed to add event for " << sock << std::endl;
      memcached_protocol_client_destroy(c);
      closesocket(sock);
    }
  }
}

/**
 * Create a socket and bind it to a specific port number
 * @param port the port number to bind to
 */
static int server_socket(const char *port)
{
  struct addrinfo *ai;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_flags= AI_PASSIVE;
  hints.ai_family= AF_UNSPEC;
  hints.ai_socktype= SOCK_STREAM;

  int error= getaddrinfo("127.0.0.1", port, &hints, &ai);
  if (error != 0)
  {
    if (error != EAI_SYSTEM)
    {
      std::cerr << "getaddrinfo(): " << gai_strerror(error) << std::endl;
    }
    else
    {
      std::cerr << "getaddrinfo(): " << strerror(errno) << std::endl;
    }

    return 0;
  }

  struct linger ling= {0, 0};

  for (struct addrinfo *next= ai; next; next= next->ai_next)
  {
    memcached_socket_t sock= socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sock == INVALID_SOCKET)
    {
      std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
      continue;
    }

    int flags;
#ifdef WIN32
    u_long arg = 1;
    if (ioctlsocket(sock, FIONBIO, &arg) == SOCKET_ERROR)
    {
      std::cerr << "Failed to set nonblocking io: " << strerror(errno) << std::endl;
      closesocket(sock);
      continue;
    }
#else
    flags= fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
      std::cerr << "Failed to get socket flags: " << strerror(errno) << std::endl;
      closesocket(sock);
      continue;
    }

    if ((flags & O_NONBLOCK) != O_NONBLOCK)
    {
      if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
      {
        std::cerr << "Failed to set socket to nonblocking mode: " << strerror(errno) << std::endl;
        closesocket(sock);
        continue;
      }
    }
#endif

    flags= 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&flags, sizeof(flags)) != 0)
    {
      std::cerr << "Failed to set SO_REUSEADDR: " << strerror(errno) << std::endl;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&flags, sizeof(flags)) != 0)
    {
      std::cerr << "Failed to set SO_KEEPALIVE: " << strerror(errno) << std::endl;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling)) != 0)
    {
      std::cerr << "Failed to set SO_LINGER: " << strerror(errno) << std::endl;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags)) != 0)
    {
      std::cerr << "Failed to set TCP_NODELAY: " << strerror(errno) << std::endl;
    }

    if (bind(sock, next->ai_addr, next->ai_addrlen) == SOCKET_ERROR)
    {
      if (get_socket_errno() != EADDRINUSE)
      {
        std::cerr << "bind(): " << strerror(errno) << std::endl;
        freeaddrinfo(ai);
      }
      closesocket(sock);
      continue;
    }

    if (listen(sock, 1024) == SOCKET_ERROR)
    {
      std::cerr << "listen(): " << strerror(errno) << std::endl;
      closesocket(sock);
      continue;
    }

    if (global_options.is_verbose)
    {
      std::cout << "Listening to " << port << std::endl;
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
  {
    return text[cmd];
  }

  return NULL;
}

/**
 * Print out the command we are about to execute
 */
static void pre_execute(const void *cookie,
                        protocol_binary_request_header *header)
{
  if (global_options.is_verbose)
  {
    if (header)
    {
      const char *cmd= comcode2str(header->request.opcode);
      if (cmd != NULL)
      {
        std::cout << "pre_execute from " << cookie << ": " << cmd << std::endl;
      }
      else
      {
        std::cout << "pre_execute from " << cookie << ": " << header->request.opcode << std::endl;
      }
    }
    else
    {
      std::cout << "pre_execute from " << cookie << std::endl;
    }
  }
}

/**
 * Print out the command we just executed
 */
static void post_execute(const void *cookie,
                         protocol_binary_request_header *header)
{
  if (global_options.is_verbose)
  {
    if (header)
    {
      const char *cmd= comcode2str(header->request.opcode);
      if (cmd != NULL)
      {
        std::cout << "post_execute from " << cookie << ": " << cmd << std::endl;
      }
      else
      {
        std::cout << "post_execute from " << cookie << ": " << header->request.opcode << std::endl;
      }
    }
    else
    {
      std::cout << "post_execute from " << cookie << std::endl;
    }
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
  protocol_binary_response_no_extras response;
  memset(&response, 0, sizeof(protocol_binary_response_no_extras));

  response.message.header.response.magic= PROTOCOL_BINARY_RES;
  response.message.header.response.opcode= header->request.opcode;
  response.message.header.response.status= htons(PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND);
  response.message.header.response.opaque= header->request.opaque;

  return response_handler(cookie, header, (protocol_binary_response_header*)&response);
}

/**
 * Program entry point. Bind to the specified port(s) and serve clients
 *
 * @param argc number of items in the argument vector
 * @param argv argument vector
 * @return EXIT_SUCCESS on success, 1 otherwise
 */
int main(int argc, char **argv)
{
  memcached_binary_protocol_callback_st *interface= &interface_v0_impl;

  event_base= event_init();
  if (event_base == NULL)
  {
    std::cerr << "Failed to create an instance of libevent" << std::endl;
    return EXIT_FAILURE;
  }

  /*
   * We need to initialize the handlers manually due to a bug in the
   * warnings generated by struct initialization in gcc (all the way up to 4.4)
   */
  initialize_interface_v0_handler();
  initialize_interface_v1_handler();

  {
    enum long_option_t {
      OPT_HELP,
      OPT_VERBOSE,
      OPT_PROTOCOL_VERSION,
      OPT_VERSION,
      OPT_PORT,
      OPT_MAX_CONNECTIONS,
      OPT_PIDFILE
    };

    static struct option long_options[]=
    {
      {"help", no_argument, NULL, OPT_HELP},
      {"port", required_argument, NULL, OPT_PORT},
      {"verbose", no_argument, NULL, OPT_VERBOSE},
      {"protocol", required_argument, NULL, OPT_PROTOCOL_VERSION},
      {"version", no_argument, NULL, OPT_VERSION},
      {"max-connections", required_argument, NULL, OPT_MAX_CONNECTIONS},
      {"pid-file", required_argument, NULL, OPT_PIDFILE},
      {0, 0, 0, 0}
    };

    int option_index;
    bool has_port= false;
    bool done= false;
    while (done == false)
    {
      switch (getopt_long(argc, argv, "", long_options, &option_index))
      {
      case -1:
        done= true;
        break;

      case OPT_PROTOCOL_VERSION:
        interface= &interface_v1_impl;
        break;

      case OPT_PIDFILE:
        global_options.pid_file= strdup(optarg);
        break;

      case OPT_VERBOSE:
        global_options.is_verbose= true;
        break;

      case OPT_PORT:
        has_port= true;
        (void)server_socket(optarg);
        break;

      case OPT_MAX_CONNECTIONS:
        maxconns= atoi(optarg);
        break;

      case OPT_HELP:  /* FALLTHROUGH */
        std::cout << "Usage: " << argv[0] << std::endl;
        for (struct option *ptr_option= long_options; ptr_option->name; ptr_option++)
        {
          std::cout << "\t" << ptr_option->name << std::endl;
        }
        return EXIT_SUCCESS;

      default:
        {
          std::cerr << "Unknown option: " << optarg << std::endl;
          return EXIT_FAILURE;
        }
      }
    }

    if (has_port == false)
    {
      (void)server_socket("9999");
    }
  }

  if (! initialize_storage())
  {
    /* Error message already printed */
    return EXIT_FAILURE;
  }

  if (global_options.pid_file)
  {
    FILE *pid_file;
    uint32_t pid;

    pid_file= fopen(global_options.pid_file, "w+");

    if (pid_file == NULL)
    {
      perror(strerror(get_socket_errno()));
      abort();
    }

    pid= (uint32_t)getpid();
    if (global_options.is_verbose)
    {
      std::cout << "pid:" << pid << std::endl;
    }
    fclose(pid_file);
  }

  if (num_server_sockets == 0)
  {
    std::cerr << "No server sockets are available" << std::endl;
    return EXIT_FAILURE;
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
    std::cerr << "Failed to allocate protocol handle" << std::endl;
    return EXIT_FAILURE;
  }

  socket_userdata_map= (struct connection*)calloc((size_t)(maxconns), sizeof(struct connection));
  if (socket_userdata_map == NULL)
  {
    std::cerr << "Failed to allocate room for connections" << std::endl;
    return EXIT_FAILURE;
  }

  memcached_binary_protocol_set_callbacks(protocol_handle, interface);
  memcached_binary_protocol_set_pedantic(protocol_handle, true);

  for (int xx= 0; xx < num_server_sockets; ++xx)
  {
    struct connection *conn= &socket_userdata_map[server_sockets[xx]];
    conn->userdata= protocol_handle;

    event_set(&conn->event, (intptr_t)server_sockets[xx], EV_READ | EV_PERSIST, accept_handler, conn);

    event_base_set(event_base, &conn->event);
    if (event_add(&conn->event, 0) == -1)
    {
      std::cerr << "Failed to add event for " << server_sockets[xx] << std::endl;
      closesocket(server_sockets[xx]);
    }
  }

  /* Serve all of the clients */
  event_base_loop(event_base, 0);

  /* NOTREACHED */
  return EXIT_SUCCESS;
}
