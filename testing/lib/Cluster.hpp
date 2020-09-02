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

  Cluster(Cluster &&c) = default;
  Cluster &operator = (Cluster &&c) = default;

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
