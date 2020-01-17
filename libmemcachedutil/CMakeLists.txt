
find_package(Threads)

add_library(memcachedutil
        ../libmemcached/backtrace.cc
        flush.cc
        pid.cc
        ping.cc
        pool.cc
        version.cc
        )

target_compile_definitions(memcachedutil PRIVATE -DBUILDING_LIBMEMCACHED)

target_link_libraries(memcachedutil memcached ${CMAKE_THREAD_LIBS_INIT})
