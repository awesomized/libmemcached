if(NOT BUILD_TESTING OR NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    return()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
include(Catch)
include(CheckTbb)

if(NOT MEMCACHED_BINARY)
    message(SEND_ERROR "Could not find memcached(1) binary")
    set(ENV{INVALID_CONFIGURATION} 1)
endif()

check_decl(pipe2 unistd.h)
check_decl(SOCK_NONBLOCK sys/socket.h)
check_decl(SOCK_CLOEXEC sys/socket.h)
check_decl(waitid sys/wait.h)
if(HAVE_WAITID)
    check_compiles(HAVE_WAITID_NOWAIT "siginfo_t s; waitid(P_ALL, 0, &s, WNOWAIT|WEXITED);" sys/types.h sys/wait.h)
endif()

file(GLOB_RECURSE TESTING_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
set(TESTING_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set(SOURCES_ROOT ${CMAKE_SOURCE_DIR})
set_source_files_properties(main.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
add_executable(runtests ${TESTING_SRC})
set_target_properties(runtests PROPERTIES CXX_STANDARD 17)
target_include_directories(runtests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src)
target_link_libraries(runtests PRIVATE libhashkit libmemcachedinternal libmemcachedutil)

if(NOT (thread IN_LIST ENABLE_SANITIZERS))
    check_tbb()
    if(HAVE_TBB)
        target_link_libraries(runtests PRIVATE ${LIBTBB})
    endif()
endif()

add_dependencies(runtests ${CLIENTS})
if(TARGET memaslap)
    set(HAVE_MEMASLAP 1)
    add_dependencies(runtests memaslap)
endif()

configure_file(conf.h.in conf.h @ONLY)

catch_discover_tests(runtests
        TEST_SPEC "lib*")
catch_discover_tests(runtests
        TEST_SPEC "hashkit*")
catch_discover_tests(runtests
        TEST_SPEC "memcached*")
catch_discover_tests(runtests
        TEST_SPEC "bin/*")
