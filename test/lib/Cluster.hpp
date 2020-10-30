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
#include "Server.hpp"

class Cluster {
public:
  explicit Cluster(Server serv, size_t cnt = 3);
  ~Cluster();

  Cluster(const Cluster &c) = delete;
  Cluster &operator=(const Cluster &c) = delete;

  Cluster(Cluster &&c) noexcept
  : proto{} {
    *this = move(c);
  };
  Cluster &operator=(Cluster &&c) noexcept {
    count = exchange(c.count, 0);
    proto = exchange(c.proto, Server{});
    cluster = exchange(c.cluster, {});
    pids = exchange(c.pids, {});
    return *this;
  }

  const vector<Server> &getServers() const;

  bool start();
  void stop(bool graceful = false);
  bool isStopped();
  bool isListening() const;
  bool ensureListening();
  void wait();
  bool restart();

private:
  size_t count;
  Server proto;
  vector<Server> cluster;
  map<pid_t, Server *> pids;
};
