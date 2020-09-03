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

  ReturnMatcher(ReturnMatcher &&rm) {
    *this = move(rm);
  }
  ReturnMatcher &operator = (ReturnMatcher &&rm) {
    memc = exchange(rm.memc, nullptr);
    expected = rm.expected;
    return *this;
  }

  bool match(const memcached_return_t &arg) const override {
    return arg == expected;
  }

  ReturnMatcher success() {
    return ReturnMatcher{memc};
  }

  ReturnMatcher operator () (memcached_return_t expected_) {
    return ReturnMatcher{memc, expected_};
  }

protected:
  string describe() const override {
    return string{"is "}
           + to_string(expected)
           + " (" + memcached_strerror(memc, expected) + ") "
           + "\n\tlast error: "
      + memcached_last_error_message(memc);
  }

private:
  const memcached_st *memc;
  memcached_return_t expected;
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

  MemcachedCluster(MemcachedCluster &&mc)
  : cluster{Server{}}
  {
    *this = move(mc);
  };
  MemcachedCluster &operator=(MemcachedCluster &&mc)
  {
    cluster = move(mc.cluster);
    memcached_clone(&memc, &mc.memc);
    returns = ReturnMatcher{&memc};
    return *this;
  }

  void enableBinaryProto(bool enable = true);
  void flush();

  static MemcachedCluster mixed();
  static MemcachedCluster network();
  static MemcachedCluster socket();

private:
  static const memcached_st empty_memc;

  void init();
};
