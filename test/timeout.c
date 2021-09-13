#include "mem_config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif
#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif

// $0 -u nobody -p <port>
int main(int argc, char **argv) {
  short port = argc == 5 ? atoi(argv[4]) : 11211;
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(struct sockaddr_in));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(port);

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  listen(listen_fd, 10);
  printf("Listening (%d) on port %d\n", listen_fd, port);

  int comm_fd = accept(listen_fd, NULL, NULL);
  printf("Connection (%d) accepted, now do nothing...\n", comm_fd);

  pause();
}
