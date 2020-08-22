#pragma once

#include "WaitForConn.hpp"

#include <csignal>

#include <functional>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

using namespace std;

class Server {
  friend class Cluster;

public:
  using str_args_t = vector<string>;
  using dyn_args_t = unordered_map<string, function<string(string)>>;
  using socket_or_port_t = variant<string, int>;

private:
  string binary;
  str_args_t str_args;
  dyn_args_t dyn_args;
  pid_t pid = 0;

  int status = 0;
  unordered_map<int, unsigned> signalled;
  socket_or_port_t socket_or_port{11211};

public:
  explicit
  Server(string &&binary_, str_args_t &&str_args_ = {}, dyn_args_t &&dyn_args_ = {})
      : binary{binary_}
      , str_args{str_args_}
      , dyn_args{dyn_args_}
  {}

  Server(string &&binary_, dyn_args_t &&dyn_args_)
      : binary{binary_}
      , str_args{}
      , dyn_args{dyn_args_}
  {}

  ~Server() {
    stop();
    wait();
  }

  Server &operator = (const Server &s) = default;
  Server(const Server &s) = default;

  Server &operator = (Server &&s) = default;
  Server(Server &&s) = default;

  pid_t getPid() const {
    return pid;
  }

  const string &getBinary() const {
    return binary;
  }

  const socket_or_port_t &getSocketOrPort() const {
    return socket_or_port;
  }

  optional<pid_t> start();
  bool stop();

  bool signal(int signo = SIGTERM);
  bool check();
  bool isListening(int max_timeout = 1000);

  bool wait(int flags = 0);
  bool tryWait();

private:
  [[nodiscard]]
  auto createArgv();

  [[nodiscard]]
  optional<WaitForConn::conn_t> createSocket();
};
