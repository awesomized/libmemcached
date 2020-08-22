#include "Cluster.hpp"

#include <sys/wait.h>

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
    if (started &= pid.has_value()) {
      pids[*pid] = &server;
    }
  }

  return started;
}

void Cluster::stop() {
  for (auto &server : cluster) {
    server.stop();
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

bool Cluster::isListening(int max_timeout) {
  vector<WaitForConn::conn_t> conns;

  for (auto &server : cluster) {
    auto conn = server.createSocket();
    if (!conn) {
      return false;
    }
    conns.emplace_back(conn.value());
  }

  WaitForConn wait_for_conn{
    std::move(conns),
    Poll{POLLOUT, 2, max_timeout}
  };
  return wait_for_conn();
}

void Cluster::wait() {
  siginfo_t inf;

  while (!isStopped()) {
    if (waitid(P_ALL, 0, &inf, WEXITED | WNOWAIT)) {
      perror("Cluster::wait waitid()");
      return;
    }

    if (pids[inf.si_pid]) {
      pids[inf.si_pid]->wait();
    }
  }
}
