
find_package(Threads)

add_library(libmemcachedutil SHARED
        ../libmemcached/backtrace.cc
        flush.cc
        pid.cc
        ping.cc
        pool.cc
        version.cc
        )
add_library(memcachedutil ALIAS libmemcachedutil)
set_target_properties(libmemcachedutil PROPERTIES LIBRARY_OUTPUT_NAME memcachedutil)
target_include_directories(libmemcachedutil PRIVATE ..)
target_compile_definitions(libmemcachedutil PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcachedutil libmemcached ${CMAKE_THREAD_LIBS_INIT})
