if(NOT BUILD_TESTING OR NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    return()
endif()

configure_init(${CMAKE_CURRENT_BINARY_DIR}/conf.h)
configure_append("#define MEMCACHED_BINARY getenv_else(\"MEMCACHED_BINARY\", \"@MEMCACHED_BINARY@\")\n")
configure_define_string(TESTING_ROOT)
configure_define_string(SOURCES_ROOT)
configure_define_string(LIBMEMCACHED_WITH_SASL_PWDB)
configure_define_string(LIBMEMCACHED_WITH_SASL_CONF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
include(Catch)

check_symbol(pipe2 unistd.h)
check_symbol(SOCK_NONBLOCK sys/socket.h)
check_symbol(SOCK_CLOEXEC sys/socket.h)
check_include(sys/wait.h)
check_symbol(waitid sys/wait.h)
if(HAVE_WAITID)
    check_c_source("
            #include <sys/types.h>
            #include <sys/wait.h>
            int main() {
                siginfo_t s;
                return waitid(P_ALL, 0, &s, WNOWAIT|WEXITED);
            }"
            HAVE_WAITID_NOWAIT
    )
endif()
if(NOT HAVE_SYS_WAIT_H)
    message(SEND_ERROR "Could not find header <sys/wait.h>")
    set(ENV{INVALID_CONFIGURATION} 1)
endif()

# memcached(1)
if(NOT MEMCACHED_BINARY)
    find_package(Memcached)
    set(MEMCACHED_BINARY ${MEMCACHED_EXECUTABLE})
endif()
if(NOT MEMCACHED_BINARY)
    message(SEND_ERROR "Could not find memcached(1) binary")
    set(ENV{INVALID_CONFIGURATION} 1)
endif()

add_executable(timeout timeout.c)
target_include_directories(timeout PRIVATE ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE TESTING_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
set(TESTING_ROOT "${CMAKE_CURRENT_BINARY_DIR}")
set(SOURCES_ROOT "${CMAKE_SOURCE_DIR}")
set_source_files_properties(main.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
add_executable(runtests ${TESTING_SRC})
set_target_properties(runtests PROPERTIES CXX_STANDARD 17)
target_include_directories(runtests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src)
target_link_libraries(runtests PRIVATE libhashkit libmemcachedinternal libmemcachedutil)

# parallelism
if(NOT (thread IN_LIST ENABLE_SANITIZERS))
    include(CheckCpp17Parallelism)
    if(HAVE_CPP17_PARALLELISM)
        target_link_libraries(runtests PRIVATE cpp17::parallelism)
    endif()
endif()

add_dependencies(runtests ${CLIENTS} timeout)
if(TARGET memaslap)
    configure_set(HAVE_MEMASLAP 1)
    add_dependencies(runtests memaslap)
endif()

configure_file(${CONFIGURE_FILE_IN} ${CONFIGURE_FILE_OUT} @ONLY)

catch_discover_tests(runtests TEST_SPEC "lib*,hashkit*,memcached*,bin/*")
