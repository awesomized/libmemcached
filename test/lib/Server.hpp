/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#pragma once

#include "common.hpp"

#include <csignal>
#include <unordered_map>

class Server {
public:
  friend class Cluster;

  using arg_func_t = function<string(string)>;
  using arg_t = variant<string, arg_func_t>;
  using arg_pair_t = pair<arg_t, arg_t>;
  using argv_t = vector<variant<arg_t, arg_pair_t>>;

  explicit Server(string binary_ = "false", argv_t args_ = {});

  ~Server();

  Server(const Server &s);
  Server &operator=(const Server &s);

  Server(Server &&s) noexcept {
    *this = move(s);
  };
  Server &operator=(Server &&s) noexcept {
    binary = exchange(s.binary, "false");
    args = exchange(s.args, {});
    pid = exchange(s.pid, 0);
    pipe = exchange(s.pipe, -1);
    status = exchange(s.status, 0);
    signalled = exchange(s.signalled, {});
    socket_or_port = exchange(s.socket_or_port, {});
    output = exchange(s.output, {});
    return *this;
  };

  pid_t getPid() const;
  int getPipe() const;
  const string &getBinary() const;
  const argv_t &getArgs() const;
  const socket_or_port_t &getSocketOrPort() const;

  struct ChildProc {
    pid_t pid;
    int pipe;
    ChildProc(pid_t pid_, int pipe_)
    : pid{pid_}
    , pipe{pipe_} {}
  };
  optional<ChildProc> start();
  bool stop();

  bool signal(int signo = SIGTERM);
  bool check();
  bool isListening() const;
  bool ensureListening();

  bool wait(int flags = 0);
  bool tryWait();
  string &drain();

private:
  string binary;
  argv_t args;
  bool sasl = false;
  pid_t pid = 0;
  int pipe = -1;
  int status = 0;
  unordered_map<int, unsigned> signalled;
  socket_or_port_t socket_or_port = 11211;
  string output;

  [[nodiscard]] vector<char *> createArgv();
  optional<string> handleArg(vector<char *> &arr, const string &arg, const arg_func_t &next_arg);
};

inline ostream &operator<<(ostream &out, const socket_or_port_t &sop) {
  if (holds_alternative<string>(sop)) {
    out << get<string>(sop);
  } else {
    out << ":" << get<int>(sop);
  }
  return out;
}

inline ostream &operator<<(ostream &out, const Server &server) {
  out << "Server{binary=" << server.getBinary() << ",pid=" << server.getPid()
      << ",conn=" << server.getSocketOrPort() << "}";
  return out;
}
