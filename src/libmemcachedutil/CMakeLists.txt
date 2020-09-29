
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
set_target_properties(libmemcachedutil PROPERTIES
        LIBRARY_OUTPUT_NAME memcachedutil
        LIBRARY_OUTPUT_NAME_DEBUG memcachedutil-dbg
        SOVERSION ${LIBMEMCACHEDUTIL_SO_VERSION}
        VERSION v${LIBMEMCACHEDUTIL_VERSION})
target_compile_definitions(libmemcachedutil PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcachedutil PUBLIC Threads::Threads)
target_include_directories(libmemcachedutil PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedutil PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS libmemcachedutil EXPORT libmemcachedutil-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcachedutil-targets NAMESPACE libmemcached::)
install(EXPORT libmemcachedutil-targets NAMESPACE libmemcached:: DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)
