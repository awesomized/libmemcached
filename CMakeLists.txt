cmake_minimum_required(VERSION 3.9...3.18)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(CMakeVersions.txt)

project(libmemcached-awesome
        VERSION "${LIBMEMCACHED_VERSION}"
        DESCRIPTION "libmemcached-awesome, a C/C++ memcached client library"
        )
set(PROJECT_HOMEPAGE_URL "https://github.com/awesomized/libmemcached")
set(PROJECT_CONTACT "Michael Wallner <mike@php.net>")

set(CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(GLOBAL_DEFINITIONS _GNU_SOURCE)

include(CMakeConfig.txt)
include(GNUInstallDirs)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

if(BUILD_DOCS OR BUILD_DOCSONLY)
    add_subdirectory(docs)
endif()

if(NOT BUILD_DOCSONLY)
    include(CMake/_Include.cmake)

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
    add_subdirectory(contrib)
    add_subdirectory(support)

    # tests need c++17 support
    add_subdirectory(test)

    # keep last
    configure_file(${CONFIGURE_FILE_IN} ${CONFIGURE_FILE_OUT} @ONLY)
endif()

list(APPEND PROJECT_CONFIG ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR})
list(APPEND PROJECT_CONFIG ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})
list(APPEND PROJECT_CONFIG ${CMAKE_BUILD_TYPE})

include(CPack.txt)

install(FILES
            AUTHORS
            BUGS.md
            ChangeLog-0.md
            ChangeLog-1.0.md
            ChangeLog-1.1.md
            CONTRIBUTING.md
            LICENSE
            NEWS
            README.md
            TODO
        COMPONENT doc
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/
        )
if(NOT WIN32)
    # skip links on windows (cmake bug?)
    install(FILES
                ChangeLog
                ChangeLog.md
                COPYING
            COMPONENT doc
            DESTINATION ${CMAKE_INSTALL_DOCDIR}/
            )
endif()

if(ENV{INVALID_CONFIGURATION})
    message(FATAL_ERROR "invalid configuration -- giving up")
endif()
