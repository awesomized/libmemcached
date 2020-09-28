#include "MemcachedCluster.hpp"
#include "Retry.hpp"

const memcached_st MemcachedCluster::empty_memc{};

void MemcachedCluster::init() {
  REQUIRE(cluster.start());

  Retry cluster_is_listening([this]() {
    return cluster.isListening();
  });
  REQUIRE(cluster_is_listening());

  REQUIRE(memcached_create(&memc));
  for (const auto &server : cluster.getServers()) {
    auto target = server.getSocketOrPort();
    if (holds_alternative<string>(target)) {
      REQUIRE(MEMCACHED_SUCCESS == memcached_server_add_unix_socket(&memc, get<string>(target).c_str()));
    } else {
      REQUIRE(MEMCACHED_SUCCESS == memcached_server_add(&memc, "localhost", get<int>(target)));
    }
  }

}

MemcachedCluster::~MemcachedCluster() {
  if (memcmp(&memc, &empty_memc, sizeof(memc))) {
    memcached_free(&memc);
  }
}

void MemcachedCluster::flush() {
  REQUIRE(MEMCACHED_SUCCESS == memcached_flush(&memc, 0));
}

MemcachedCluster::MemcachedCluster()
: cluster{Server{
  MEMCACHED_BINARY,
  {random_socket_or_port_arg()}
}}
{
  init();
}

MemcachedCluster::MemcachedCluster(Cluster &&cluster_)
: cluster{move(cluster_)}
{
  init();
}

MemcachedCluster::MemcachedCluster(MemcachedCluster &&mc)
    : cluster{Server{}}
{
  *this = move(mc);
}

MemcachedCluster &MemcachedCluster::operator=(MemcachedCluster &&mc) {
  cluster = move(mc.cluster);
  memcached_clone(&memc, &mc.memc);
  returns = ReturnMatcher{&memc};
  return *this;
}

MemcachedCluster MemcachedCluster::mixed() {
  return MemcachedCluster{};
}

MemcachedCluster MemcachedCluster::network() {
  return MemcachedCluster{Cluster{Server{
    MEMCACHED_BINARY,
    {"-p", random_socket_or_port_string}
  }}};
}

MemcachedCluster MemcachedCluster::socket() {
  return MemcachedCluster{Cluster{Server{
    MEMCACHED_BINARY,
    {"-s", random_socket_or_port_string}
  }}};
}

#if LIBMEMCACHED_WITH_SASL_SUPPORT
MemcachedCluster MemcachedCluster::sasl() {
  auto mc = MemcachedCluster{Cluster{Server{
      MEMCACHED_BINARY,
      {
        Server::arg_pair_t{"-p", random_socket_or_port_string},
        Server::arg_t{"-S"}
      }
  }}};
  mc.enableBinaryProto();
  REQUIRE(MEMCACHED_SUCCESS == memcached_set_sasl_auth_data(&mc.memc,
      "memcached", "memcached"));
  return mc;
}
#endif

void MemcachedCluster::enableBinaryProto(bool enable) {
  REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set(&memc,
      MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, enable));
}

void MemcachedCluster::enableBuffering(bool enable) {
  REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set(&memc,
      MEMCACHED_BEHAVIOR_BUFFER_REQUESTS, enable));
}

void MemcachedCluster::enableReplication() {
  REQUIRE(MEMCACHED_SUCCESS == memcached_behavior_set(&memc,
      MEMCACHED_BEHAVIOR_NUMBER_OF_REPLICAS, memcached_server_count(&memc)));
}

