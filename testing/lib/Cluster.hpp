#pragma once

#include "Server.hpp"

#include <map>
#include <thread>

class Cluster {
private:
  uint16_t count;
  Server proto;
  vector<Server> cluster;

  map<pid_t, Server *> pids;

public:
  explicit
  Cluster(Server &&serv, uint16_t cnt = 0)
  : count{cnt}
  , proto{serv}
  {
    if (!cnt) {
      count = thread::hardware_concurrency() ?: 4;
    }
    reset();
  }

  bool start();
  void stop();
  void reset();
  bool isStopped();
  bool isListening(int max_timeout = 1000);
  void wait();
};
