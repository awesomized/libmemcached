#pragma once

#include "common.hpp"
#include "Cluster.hpp"

class MemcachedCluster {
public:
  Cluster cluster;
  memcached_st memc;

  MemcachedCluster();
  explicit
  MemcachedCluster(Cluster &&cluster);
  ~MemcachedCluster();

  void enableBinary(bool enable = true);
  void flush();

  static MemcachedCluster mixed();
  static MemcachedCluster net();
  static MemcachedCluster socket();

private:
  void init();
};
