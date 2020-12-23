#pragma once

#include "libmemcached-1.0/platform.h"

#if defined __cplusplus
# include <cerrno>
#else
# include <errno.h>
#endif

#include <fcntl.h>

#if defined HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#if defined HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#if defined HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

#if defined HAVE_NETINET_TCP_H
#  include <netinet/tcp.h>
#endif

#if defined HAVE_SYS_UN_H
# include <sys/un.h>
#endif

#if defined HAVE_UNISTD_H
# include <unistd.h>
#endif

#if defined __cplusplus
extern "C" {
#endif

# if defined _WIN32
# define P9Y_NEED_GET_SOCKET_ERRNO
int get_socket_errno();
#  define SHUT_WR SD_SEND
#  define SHUT_RD SD_RECEIVE
#  define SHUT_RDWR SD_BOTH
# else
#   define closesocket close
#   define get_socket_errno() errno
#   define INVALID_SOCKET (-1)
#   define SOCKET_ERROR (-1)
# endif

#if defined __cplusplus
}
#endif

#if !defined SOCK_CLOEXEC
#  define SOCK_CLOEXEC 0
#endif

#if !defined SOCK_NONBLOCK
#  define SOCK_NONBLOCK 0
#endif

#if !defined FD_CLOEXEC
#  define FD_CLOEXEC 0
#endif

#if !defined SO_NOSIGPIPE
#  define SO_NOSIGPIPE 0
#endif

#if !defined TCP_NODELAY
#  define TCP_NODELAY 0
#endif

#if !defined TCP_KEEPIDLE
#  define TCP_KEEPIDLE 0
#endif

#if !defined EAI_SYSTEM
#  define EAI_SYSTEM (-1)
#endif

#if !defined HAVE_MSG_NOSIGNAL
#  define MSG_NOSIGNAL 0
#endif

#if !defined HAVE_MSG_MORE
#  define MSG_MORE 0
#endif
