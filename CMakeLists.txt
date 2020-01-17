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

configure_file(mem_config.h.in ${CMAKE_BINARY_DIR}/mem_config.h @ONLY)

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(clients)
add_subdirectory(libhashkit)
add_subdirectory(libhashkit-1.0)
add_subdirectory(libmemcached)
add_subdirectory(libmemcached-1.0)
add_subdirectory(libmemcachedinternal)
add_subdirectory(libmemcachedutil)


















include_directories(${CMAKE_BINARY_DIR})
include_directories(.)
include_directories(clients)
include_directories(example)
include_directories(libhashkit)
include_directories(libhashkit-1.0)
include_directories(libmemcached)
include_directories(libmemcached/csl)
include_directories(libmemcached/memcached)
include_directories(libmemcached-1.0)
include_directories(libmemcached-1.0/struct)
include_directories(libmemcached-1.0/types)
include_directories(libmemcachedprotocol)
include_directories(libmemcachedprotocol-0.0)
include_directories(libmemcachedutil)
include_directories(libmemcachedutil-1.0)
include_directories(libtest)
include_directories(libtest/exception)
include_directories(libtest/result)
include_directories(tests)
include_directories(tests/libmemcached-1.0)
include_directories(util)
include_directories(win32)


if (FALSE)
    add_executable(nope
        example/byteorder.cc
        example/byteorder.h
        example/interface_v0.cc
        example/interface_v1.cc
        example/memcached_light.cc
        example/memcached_light.h
        example/storage.cc
        example/storage.h


        libmemcached-1.0/t/c_sasl_test.c
        libmemcached-1.0/t/c_test.c
        libmemcached-1.0/t/cc_test.cc

        libmemcachedprotocol/ascii_handler.c
        libmemcachedprotocol/ascii_handler.h
        libmemcachedprotocol/binary_handler.c
        libmemcachedprotocol/binary_handler.h
        libmemcachedprotocol/cache.c
        libmemcachedprotocol/cache.h
        libmemcachedprotocol/common.h
        libmemcachedprotocol/handler.c
        libmemcachedprotocol/pedantic.c
        libmemcachedprotocol-0.0/binary.h
        libmemcachedprotocol-0.0/callback.h
        libmemcachedprotocol-0.0/handler.h
        libmemcachedprotocol-0.0/vbucket.h

        libmemcachedutil/common.h

        libmemcachedutil-1.0/flush.h
        libmemcachedutil-1.0/ostream.hpp
        libmemcachedutil-1.0/pid.h
        libmemcachedutil-1.0/ping.h
        libmemcachedutil-1.0/pool.h
        libmemcachedutil-1.0/util.h
        libmemcachedutil-1.0/version.h

        libtest/exception/disconnected.hpp
        libtest/exception/fatal.cc
        libtest/exception/fatal.hpp
        libtest/result/base.hpp
        libtest/result/fail.hpp
        libtest/result/skip.hpp
        libtest/result/success.hpp
        libtest/abort.cc
        libtest/alarm.cc
        libtest/alarm.h
        libtest/backtrace_test.cc
        libtest/binaries.cc
        libtest/binaries.h
        libtest/blobslap_worker.cc
        libtest/blobslap_worker.h
        libtest/callbacks.h
        libtest/client.cc
        libtest/client.hpp
        libtest/cmdline.cc
        libtest/cmdline.h
        libtest/collection.cc
        libtest/collection.h
        libtest/common.h
        libtest/comparison.cc
        libtest/comparison.hpp
        libtest/core.cc
        libtest/core.h
        libtest/core_count.cc
        libtest/cpu.cc
        libtest/cpu.hpp
        libtest/dns.cc
        libtest/dns.hpp
        libtest/dream.cc
        libtest/dream.h
        libtest/drizzled.cc
        libtest/drizzled.h
        libtest/error.h
        libtest/exception.cc
        libtest/exception.hpp
        libtest/failed.cc
        libtest/formatter.cc
        libtest/formatter.hpp
        libtest/framework.cc
        libtest/framework.h
        libtest/gearmand.cc
        libtest/gearmand.h
        libtest/get.h
        libtest/has.cc
        libtest/has.hpp
        libtest/http.cc
        libtest/http.hpp
        libtest/is_local.cc
        libtest/is_local.hpp
        libtest/is_pid.hpp
        libtest/killpid.cc
        libtest/killpid.h
        libtest/libtool.cc
        libtest/libtool.hpp
        libtest/lite.h
        libtest/main.cc
        libtest/memcached.cc
        libtest/memcached.h
        libtest/memcached.hpp
        libtest/poll_error.hpp
        libtest/port.cc
        libtest/port.h
        libtest/result.cc
        libtest/result.hpp
        libtest/runner.cc
        libtest/runner.h
        libtest/server.cc
        libtest/server.h
        libtest/server_container.cc
        libtest/server_container.h
        libtest/signal.cc
        libtest/signal.h
        libtest/skiptest.cc
        libtest/socket.cc
        libtest/socket.hpp
        libtest/stream.h
        libtest/strerror.cc
        libtest/strerror.h
        libtest/string.hpp
        libtest/test.h
        libtest/test.hpp
        libtest/thread.hpp
        libtest/timer.cc
        libtest/timer.hpp
        libtest/tmpfile.cc
        libtest/tmpfile.hpp
        libtest/unittest.cc
        libtest/valgrind.h
        libtest/vchar.cc
        libtest/vchar.hpp
        libtest/version.h
        libtest/visibility.h
        libtest/wait.cc
        libtest/wait.h
        libtest/yatl.h
        libtest/yatlcon.h

        tests/libmemcached-1.0/all_tests.cc
        tests/libmemcached-1.0/all_tests.h
        tests/libmemcached-1.0/all_tests_socket.cc
        tests/libmemcached-1.0/atomsmasher.cc
        tests/libmemcached-1.0/basic.cc
        tests/libmemcached-1.0/callback_counter.cc
        tests/libmemcached-1.0/callback_counter.h
        tests/libmemcached-1.0/callbacks.cc
        tests/libmemcached-1.0/debug.cc
        tests/libmemcached-1.0/deprecated.cc
        tests/libmemcached-1.0/dump.cc
        tests/libmemcached-1.0/dump.h
        tests/libmemcached-1.0/encoding_key.cc
        tests/libmemcached-1.0/encoding_key.h
        tests/libmemcached-1.0/error_conditions.cc
        tests/libmemcached-1.0/exist.cc
        tests/libmemcached-1.0/fetch_all_results.cc
        tests/libmemcached-1.0/fetch_all_results.h
        tests/libmemcached-1.0/generate.cc
        tests/libmemcached-1.0/generate.h
        tests/libmemcached-1.0/haldenbrand.cc
        tests/libmemcached-1.0/haldenbrand.h
        tests/libmemcached-1.0/internals.cc
        tests/libmemcached-1.0/ketama.cc
        tests/libmemcached-1.0/mem_functions.cc
        tests/libmemcached-1.0/mem_functions.h
        tests/libmemcached-1.0/memcached_fetch_execute.cc
        tests/libmemcached-1.0/memcached_fetch_execute.h
        tests/libmemcached-1.0/memcached_get.cc
        tests/libmemcached-1.0/memcached_get.h
        tests/libmemcached-1.0/namespace.cc
        tests/libmemcached-1.0/parser.cc
        tests/libmemcached-1.0/parser.h
        tests/libmemcached-1.0/plus.cpp
        tests/libmemcached-1.0/pool.cc
        tests/libmemcached-1.0/print.cc
        tests/libmemcached-1.0/replication.cc
        tests/libmemcached-1.0/sasl.cc
        tests/libmemcached-1.0/server_add.cc
        tests/libmemcached-1.0/setup_and_teardowns.cc
        tests/libmemcached-1.0/setup_and_teardowns.h
        tests/libmemcached-1.0/stat.cc
        tests/libmemcached-1.0/stat.h
        tests/libmemcached-1.0/string.cc
        tests/libmemcached-1.0/touch.cc
        tests/libmemcached-1.0/virtual_buckets.cc
        tests/basic.h
        tests/callbacks.h
        tests/cpp_example.cc
        tests/cycle.cc
        tests/debug.h
        tests/deprecated.h
        tests/error_conditions.h
        tests/exist.h
        tests/failure.cc
        tests/hash_plus.cc
        tests/hash_results.h
        tests/hashkit_functions.cc
        tests/ketama.h
        tests/ketama_test_cases.h
        tests/ketama_test_cases_spy.h
        tests/keys.hpp
        tests/libmemcached_test_container.h
        tests/libmemcached_world.h
        tests/libmemcached_world_socket.h
        tests/mem_udp.cc
        tests/memc.hpp
        tests/memcapable.cc
        tests/memcat.cc
        tests/memcp.cc
        tests/memdump.cc
        tests/memerror.cc
        tests/memexist.cc
        tests/memflush.cc
        tests/memping.cc
        tests/memrm.cc
        tests/memslap.cc
        tests/memstat.cc
        tests/memtouch.cc
        tests/namespace.h
        tests/parser.cc
        tests/pool.h
        tests/print.h
        tests/replication.h
        tests/runner.h
        tests/server_add.h
        tests/string.h
        tests/touch.h
        tests/virtual_buckets.h

        util/daemon.cc
        util/daemon.hpp
        util/instance.cc
        util/instance.hpp
        util/log.hpp
        util/logfile.cc
        util/logfile.hpp
        util/operation.cc
        util/operation.hpp
        util/pidfile.cc
        util/pidfile.hpp
        util/signal.cc
        util/signal.hpp
        util/string.hpp

        win32/wrappers.h

            )
endif()
