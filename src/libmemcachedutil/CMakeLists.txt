
add_library(libmemcachedutil SHARED
        ../libmemcached/backtrace.cc
        common.h
        flush.cc
        pid.cc
        ping.cc
        pool.cc
        version.cc
        )
add_library(memcachedutil ALIAS libmemcachedutil)
set_target_properties(libmemcachedutil PROPERTIES LIBRARY_OUTPUT_NAME memcachedutil)
target_compile_definitions(libmemcachedutil PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcachedutil PUBLIC libmemcached Threads::Threads)
target_include_directories(libmemcachedutil PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedutil PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

set_target_properties(libmemcachedutil PROPERTIES SOVERSION ${LIBMEMCACHEDUTIL_SO_VERSION})
install(TARGETS libmemcachedutil EXPORT libmemcachedutil
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcachedutil)
install(EXPORT libmemcachedutil DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
