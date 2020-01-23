cmake_minimum_required(VERSION 3.9..3.16)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(libmemcached
        VERSION 1.99
        DESCRIPTION "libmemcached https://github.com/m6w6/libmemcached"
        )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(Config.cmake)
include(Include.cmake)

find_package(Threads REQUIRED)
find_library(HAVE_LIBSASL NAMES sasl sasl2)
check_header(sasl/sasl.h)

check_header(arpa/inet.h)
check_header(dlfcn.h)
check_header(errno.h)
check_header(execinfo.h)
check_header(fcntl.h)
check_header(io.h)
check_header(limits.h)
check_header(netdb.h)
check_header(poll.h)
check_header(stddef.h)
check_header(stdlib.h)
check_header(strings.h)
check_header(sys/socket.h)
check_header(sys/time.h)
check_header(sys/types.h)
check_header(sys/un.h)
check_header(time.h)
check_header(umem.h)
check_header(unistd.h)
check_header(winsock2.h)
check_header(ws2tcpip.h)

check_decl(fcntl fcntl.h)
check_decl(htonll arpa/inet.h)
check_decl(MSG_DONTWAIT sys/socket.h)
check_decl(MSG_MORE sys/socket.h)
check_decl(MSG_NOSIGNAL sys/socket.h)
check_decl(rcvtimeo sys/socket.h)
check_decl(sndtimeo sys/socket.h)
check_decl(strerror_r string.h)
check_decl(strerror string.h)
check_decl(abi::__cxa_demangle cxxabi.h)
set(HAVE_GCC_ABI_DEMANGLE ${HAVE_ABI____CXA_DEMANGLE})

check_type(in_port_t netinet/in.h)

check_header(cstdint)
check_header(cinttypes)

check_debug()

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(clients)
add_subdirectory(libhashkit)
add_subdirectory(libhashkit-1.0)
add_subdirectory(libmemcached)
add_subdirectory(libmemcached-1.0)
add_subdirectory(libmemcachedutil)

add_subdirectory(docs)

# keep last
configure_file(mem_config.h.in ${CMAKE_BINARY_DIR}/mem_config.h @ONLY)
