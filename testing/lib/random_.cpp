#include "random_.hpp"

#include <cstdio>
#include <iostream>
#include <sys/random.h>

unsigned random_num(unsigned min, unsigned max) {
  unsigned p;
  getrandom(&p, sizeof(p), 0);
  return (p % (max - min + 1)) + min;
}

unsigned random_port() {
  return random_num(34567, 65000);
}

string random_socket() {
  auto sock = tempnam(nullptr, "libmc");

  if (!sock) {
    perror("random_socket tempnam()");
    return {};
  }

  return sock;
}

string random_socket_or_port_string(const string &what) {
  if (what == "-s") {
    return random_socket();
  }

  return to_string(random_port());
}

string random_socket_or_port_flag() {
  return random_num(0, 1) ? "-p" : "-s";
}
