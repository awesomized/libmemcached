#include "Cluster.hpp"
#include "Retry.hpp"

#include <sys/wait.h>

Cluster::Cluster(Server serv, uint16_t cnt)
: count{cnt}
, proto{move(serv)}
{
  if (!count) {
    count = 1;
  }
  for (int i = 0; i < count; ++i) {
    cluster.push_back(proto);
  }
}

Cluster::~Cluster() {
  stop();
  wait();
}

const vector<Server> &Cluster::getServers() const {
  return cluster;
}

bool Cluster::start() {
  bool started = true;

  for (auto &server : cluster) {
    if (!startServer(server)) {
      started = false;
    }
  }

  return started;
}

void Cluster::stop(bool graceful) {
  for (auto &server : cluster) {
    server.drain();
    if (graceful) {
      server.stop();
    } else {
      // no cookies for memcached; TERM is just too slow
      server.signal(SIGKILL);
    }
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
    Retry server_is_listening{[&] {
      if (!server.isListening()) {
        // zombie?
        auto old_pid = server.getPid();
        if (server.tryWait()) {
          cerr << "Collected zombie " << server << "(old pid=" << old_pid << ")\n";
          pids.erase(old_pid);
          // restart
          startServer(server);
        }
        if (!server.isListening()) {
          return false;
        }
      }
      return true;
    }};
    if (!server_is_listening()) {
      return false;
    }
  }

  return true;
}

bool Cluster::startServer(Server &server) {
  if (server.start().has_value()) {
    pids[server.getPid()] = &server;
    return true;
  }
  return false;
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

bool Cluster::restart() {
  stop();
  wait();
  return start();
}
