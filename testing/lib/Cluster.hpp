#pragma once

#include "common.hpp"
#include "Server.hpp"

class Cluster {
public:
  explicit
  Cluster(Server &&serv, uint16_t cnt = 0);

  ~Cluster();

  Cluster(const Cluster &c) = delete;
  Cluster &operator = (const Cluster &c) = delete;

  Cluster(Cluster &&c)
  : proto{}
  {
    *this = move(c);
  };
  Cluster &operator = (Cluster &&c) {
    count = exchange(c.count, 0);
    proto = exchange(c.proto, Server{});
    cluster = exchange(c.cluster, {});
    pids = exchange(c.pids, {});
    return *this;
  }

  const vector<Server> &getServers() const;

  bool start();
  void stop();
  void reset();
  bool isStopped();
  bool isListening();
  void wait();

private:
  uint16_t count;
  Server proto;
  vector<Server> cluster;
  map<pid_t, Server *> pids;
};
