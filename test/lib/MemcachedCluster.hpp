#pragma once

#include "common.hpp"
#include "Cluster.hpp"
#include "ReturnMatcher.hpp"


class MemcachedCluster {
public:
  using behavior_t = pair<memcached_behavior_t, uint64_t>;
  using behaviors_t = vector<behavior_t>;

  Cluster cluster;
  memcached_st memc{empty_memc};
  ReturnMatcher returns{&memc};

  MemcachedCluster();
  explicit MemcachedCluster(Cluster &&cluster, behaviors_t to_set = {});
  ~MemcachedCluster();

  MemcachedCluster(const MemcachedCluster &) = delete;
  MemcachedCluster &operator=(const MemcachedCluster &) = delete;

  MemcachedCluster(MemcachedCluster &&mc);
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
  behaviors_t to_set;
  static const memcached_st empty_memc;

  void init();
};
