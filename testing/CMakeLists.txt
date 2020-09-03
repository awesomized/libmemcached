
add_executable(catch_main
        main.cpp

        lib/common.cpp

        lib/Cluster.cpp
        lib/Connection.cpp
        lib/ForkAndExec.cpp
        lib/MemcachedCluster.cpp
        lib/Server.cpp
        lib/Retry.cpp

        lib.cpp
        hashkit/basic.cpp
        memcached/basic.cpp
        memcached/callbacks.cpp
        memcached/servers.cpp
        memcached/dump.cpp
        memcached/encoding_key.cpp
        memcached/exist.cpp
        memcached/haldenbrand.cpp
        )

set_target_properties(catch_main PROPERTIES
        CXX_STANDARD 17)

target_link_libraries(catch_main libhashkit libmemcached)
