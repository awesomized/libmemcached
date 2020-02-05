cmake_minimum_required(VERSION 3.9..3.16)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(CMakeVersions.txt)

project(libmemcached
        VERSION "${LIBMEMCACHED_VERSION}"
        DESCRIPTION "libmemcached https://github.com/m6w6/libmemcached"
        )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CMakeConfig.txt)

include_directories(${CMAKE_BINARY_DIR})
set(AUTOHEADER_FILE mem_config.h)

set(CLIENTS
        memcapable
        memcat
        memcp
        memdump
        memerror
        memexist
        memflush
        memparse
        memping
        memrm
        memslap
        memstat
        memtouch
        )

add_subdirectory(clients)
add_subdirectory(libhashkit)
add_subdirectory(libhashkit-1.0)
add_subdirectory(libmemcached)
add_subdirectory(libmemcached-1.0)
add_subdirectory(libmemcachedutil)
add_subdirectory(libmemcachedutil-1.0)

if(BUILD_TESTING)
    add_subdirectory(libtest)
    add_subdirectory(tests)
endif()

add_subdirectory(docs)
add_subdirectory(support)

# keep last
configure_file(mem_config.h.in ${CMAKE_BINARY_DIR}/${AUTOHEADER_FILE} @ONLY)
