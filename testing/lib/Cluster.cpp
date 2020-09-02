#include "Cluster.hpp"

#include <sys/wait.h>

Cluster::Cluster(Server &&serv, uint16_t cnt)
: count{cnt}
, proto{forward<Server>(serv)}
{
  if (!cnt) {
    count = thread::hardware_concurrency()/2 ?: 4;
  }
  reset();
}

Cluster::~Cluster() {
  stop();
  wait();
}

const vector<Server> &Cluster::getServers() const {
  return cluster;
}

void Cluster::reset() {
  pids.clear();
  cluster.clear();
  for (int i = 0; i < count; ++i) {
    cluster.push_back(proto);
  }
}

bool Cluster::start() {
  bool started = true;

  for (auto &server : cluster) {
    auto pid = server.start();
    if (pid.has_value()) {
      pids[*pid] = &server;
    } else {
      started = false;
    }
  }

  return started;
}

void Cluster::stop() {
  for (auto &server : cluster) {
    // no cookies for memcached; TERM is just too slow
    server.signal(SIGKILL);
  }
}

bool Cluster::isStopped() {
  for (auto &server : cluster) {
    if (server.getPid() && !server.tryWait()) {
      return false;
    }
  }
  return true;
}

bool Cluster::isListening() {
  for (auto &server : cluster) {
    if (!server.isListening()) {
      // zombie?
      auto old_pid = server.getPid();
      if (server.tryWait()) {
        pids.erase(old_pid);
        auto pid = server.start();
        if (pid.has_value()) {
          pids[*pid] = &server;
        }
      }
      return server.isListening();
    }
  }

  return true;
}

void Cluster::wait() {
  siginfo_t inf;

  while (!isStopped()) {
    if (waitid(P_ALL, 0, &inf, WEXITED | WNOWAIT)) {
      perror("Cluster::wait waitid()");
      return;
    }

    auto server = pids.find(inf.si_pid);
    if (server != pids.end()) {
      server->second->wait();
    }
  }
}

