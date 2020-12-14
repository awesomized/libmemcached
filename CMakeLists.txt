cmake_minimum_required(VERSION 3.9...3.18)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(CMakeVersions.txt)

project(libmemcached
        VERSION "${LIBMEMCACHED_VERSION}"
        HOMEPAGE_URL "https://github.com/m6w6/libmemcached"
        DESCRIPTION "libmemcached, a C/C++ memcached client library"
        )
set(PROJECT_CONTACT "Michael Wallner <mike@php.net>")

set(CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CMakeConfig.txt)

if(BUILD_DOCS OR BUILD_DOCSONLY)
    add_subdirectory(docs)
endif()

if(NOT BUILD_DOCSONLY)

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

    add_subdirectory(src)
    add_subdirectory(contrib)
    add_subdirectory(include)
    add_subdirectory(support)

    # tests need c++17 support
    add_subdirectory(test)

    # keep last
    configure_file(${CONFIGURE_FILE_IN} ${CONFIGURE_FILE_OUT} @ONLY)
endif()

include(CPack.txt)

install(FILES
            AUTHORS
            BUGS.md
            ChangeLog
            ChangeLog-0.md
            ChangeLog-1.0.md
            ChangeLog-1.1.md
            ChangeLog.md
            CONTRIBUTING.md
            COPYING
            LICENSE
            NEWS
            README.md
            TODO
        COMPONENT doc
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/
        )

if(ENV{INVALID_CONFIGURATION})
    message(FATAL_ERROR "invalid configuration -- giving up")
endif()
