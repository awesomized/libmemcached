
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
target_link_libraries(libmemcachedutil libmemcached Threads::Threads)

set_target_properties(libmemcachedutil PROPERTIES SOVERSION ${LIBMEMCACHEDUTIL_SO_VERSION})
install(TARGETS libmemcachedutil EXPORT libmemcachedutil)
export(EXPORT libmemcachedutil)
install(EXPORT libmemcachedutil DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
