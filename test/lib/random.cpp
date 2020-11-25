#include "test/lib/random.hpp"
#include "test/lib/Connection.hpp"

#if HAVE_UNISTD_H
#  include <unistd.h> // getpid()
#endif
mt19937_64 rnd_eng;
mutex rnd_mtx;

void random_setup() {
  using namespace chrono;

  auto time = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
  rnd_eng.seed(static_cast<mt19937_64::result_type>(time.count()));
}

unsigned random_port() {
  do {
    auto port = random_num(5000, 32000);
    Connection conn(port);

    if (!conn.open()) {
      return port;
    }
    if (!conn.isOpen()) {
      return port;
    }
  } while(true);
}

string random_port_string(const string &) {
  return to_string(random_port());
}

string random_socket(const string &prefix) {
  return prefix + to_string(random_num(1U, UINT32_MAX)) + "@" + to_string(getpid()) + ".sock";
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

char random_ascii(char min, char max) {
  return static_cast<char>(random_num(min, max));
}

string random_ascii_string(size_t len, char min, char max) {
  string s;
  s.reserve(len + 1);

  for (size_t rem = 0; rem < len; ++rem) {
    s += random_ascii(min, max);
  }

  s[len] = '\0';
  assert(strlen(s.c_str()) == s.size());

  return s;
}

pair<string, string> random_ascii_pair(size_t minlen, size_t maxlen) {
  return {
      random_ascii_string(random_num(minlen, maxlen)),
      random_ascii_string(random_num(minlen, maxlen))
  };
}

#include <climits>

char random_binary() {
  return random_num(CHAR_MIN, CHAR_MAX);
}

string random_binary_string(size_t len) {
  string s;
  s.reserve(len + 1);

  for (size_t rem = 0; rem < len; ++rem) {
    s += random_binary();
  }
  s[len] = 0;

  return s;
}
