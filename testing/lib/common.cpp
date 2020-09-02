#include "common.hpp"
#include "Connection.hpp"

#include <cstdlib>
#include <sys/random.h>
#include <unistd.h>

unsigned random_num(unsigned min, unsigned max) {
  unsigned p;
  getrandom(&p, sizeof(p), 0);
  return (p % (max - min + 1)) + min;
}

unsigned random_port() {
  retry:
  int port = random_num(2<<9, 2<<15);
  Connection conn(port);

  if (!conn.open()) {
    return port;
  }
  if (!conn.isOpen()) {
    return port;
  }
  goto retry;
}

string random_socket() {
  return "/tmp/libmc." + to_string(random_num(1, UINT32_MAX)) + "@" + to_string(getpid()) + ".sock";
}

string random_socket_or_port_string(const string &what) {
  if (what == "-s") {
    return random_socket();
  }

  return to_string(random_port());
}

string random_socket_or_port_flag(const string &binary) {
  (void) binary;
  return random_num(0, 1) ? "-p" : "-s";
}

const char *getenv_else(const char *var, const char *defval) {
  auto val = getenv(var);
  if (val && *val) {
    return val;
  }
  return defval;
}

