#include "testing/lib/random.hpp"
#include "testing/lib/Connection.hpp"

#include <chrono>
#include <random>

#include <unistd.h> // getpid()


template<typename T>
enable_if_t<is_integral_v<T>, T> random_num(T min, T max) {
  using namespace chrono;
  using rnd = mt19937;
  using dst = uniform_int_distribution<T>;

  auto time = duration_cast<microseconds>(system_clock::now().time_since_epoch());
  auto seed = static_cast<rnd::result_type>(time.count() % numeric_limits<T>::max());
  auto rgen = rnd{seed};
  return dst(min, max)(rgen);
}

unsigned random_port() {
  retry:
  auto port = random_num(2<<9, 2<<15);
  Connection conn(port);

  if (!conn.open()) {
    return port;
  }
  if (!conn.isOpen()) {
    return port;
  }
  goto retry;
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

