#pragma once

#include "common.hpp"
#include "Cluster.hpp"

class ReturnMatcher : public Catch::MatcherBase<memcached_return_t> {
public:
  explicit ReturnMatcher(const memcached_st *memc_, memcached_return_t expected_ = MEMCACHED_SUCCESS)
      : memc{memc_}
      , expected{expected_}
  {}

  ReturnMatcher(const ReturnMatcher &) = default;
  ReturnMatcher &operator = (const ReturnMatcher &) = default;

  ReturnMatcher(ReturnMatcher &&rm);
  ReturnMatcher &operator = (ReturnMatcher &&rm);

  bool match(const memcached_return_t &arg) const override;
  ReturnMatcher success();
  ReturnMatcher operator () (memcached_return_t expected_);

protected:
  string describe() const override;

private:
  const memcached_st *memc;
  memcached_return_t expected{MEMCACHED_SUCCESS};
};

class LoneReturnMatcher {
public:
  ReturnMatcher returns;
  explicit LoneReturnMatcher(const memcached_st *memc) : returns{memc}
  {}
};

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
