#pragma once

#include "common.hpp"
#include "Cluster.hpp"
#include "ReturnMatcher.hpp"


class MemcachedCluster {
public:
  Cluster cluster;
  memcached_st memc{empty_memc};
  ReturnMatcher returns{&memc};

  MemcachedCluster();
  explicit
  MemcachedCluster(Cluster &&cluster);
  ~MemcachedCluster();

  MemcachedCluster(const MemcachedCluster &) = delete;
  MemcachedCluster &operator=(const MemcachedCluster &) = delete;

  MemcachedCluster(MemcachedCluster &&mc);;
  MemcachedCluster &operator=(MemcachedCluster &&mc);

  void enableBinaryProto(bool enable = true);
  void flush();

  static MemcachedCluster mixed();
  static MemcachedCluster network();
  static MemcachedCluster socket();

private:
  static const memcached_st empty_memc;

  void init();
};
