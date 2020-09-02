#pragma once

#include "common.hpp"

#include <csignal>

class Server {
public:

  friend class Cluster;

  using arg_func_t = function<string(string)>;
  using arg_t = variant<string, arg_func_t>;
  using arg_pair_t = pair<arg_t, arg_t>;
  using argv_t = vector<variant<arg_t, arg_pair_t>>;

  explicit
  Server(string &&binary_, argv_t && args_ = {});

  ~Server();

  Server(const Server &s);
  Server &operator = (const Server &s);

  Server &operator = (Server &&s) = default;
  Server(Server &&s) = default;

  pid_t getPid() const;

  const string &getBinary() const;

  const argv_t &getArgs() const;

  const socket_or_port_t &getSocketOrPort() const;

  optional<pid_t> start();
  bool stop();

  bool signal(int signo = SIGTERM);
  bool check();
  bool isListening();

  bool wait(int flags = 0);
  bool tryWait();

private:
  string binary;
  argv_t args;
  pid_t pid = 0;
  int status = 0;
  unordered_map<int, unsigned> signalled;
  socket_or_port_t socket_or_port = 11211;

  [[nodiscard]]
  vector<char *> createArgv();
  optional<string> handleArg(vector<char *> &arr, const string &arg, const arg_func_t &next_arg);
};
