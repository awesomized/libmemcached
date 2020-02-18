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

foreach(INCLUDE IN ITEMS ${CMAKE_BINARY_DIR} .)
    include_directories(
            ${INCLUDE}/
            ${INCLUDE}/include
            ${INCLUDE}/src
            )
endforeach()
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

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(docs)
add_subdirectory(support)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# keep last
configure_file(src/mem_config.h.in ${AUTOHEADER_FILE} @ONLY)
