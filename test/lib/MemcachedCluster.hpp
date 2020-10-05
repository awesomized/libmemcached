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
  void enableBuffering(bool enable = true);
  void enableReplication();
  void enableUdp(bool enable = true);
  void flush();

  static MemcachedCluster mixed();
  static MemcachedCluster network();
  static MemcachedCluster socket();
  static MemcachedCluster udp();

#if LIBMEMCACHED_WITH_SASL_SUPPORT
  static MemcachedCluster sasl();
#endif

  void killOneServer();

private:
  static const memcached_st empty_memc;

  void init();
};
