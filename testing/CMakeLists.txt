if(NOT BUILD_TESTING)
    return()
endif()

file(GLOB_RECURSE TESTING_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
set(TESTING_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set_source_files_properties(main.cpp PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
configure_file(conf.h.in conf.h @ONLY)
add_executable(runtests ${TESTING_SRC})
set_target_properties(runtests PROPERTIES CXX_STANDARD 17)
target_include_directories(runtests PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src)
target_link_libraries(runtests PRIVATE libhashkit libmemcached libmemcachedutil)
add_dependencies(runtests ${CLIENTS})

macro(add_test TEST_CASE)
    _add_test(${TEST_CASE} runtests ${TEST_CASE})
endmacro()

add_test("lib/Server")
add_test("lib/Cluster")

add_test("hashkit")

add_test("memcached_append")
add_test("memcached_basic")
add_test("memcached_callbacks")
add_test("memcached_cas")
add_test("memcached_dump")
add_test("memcached_encoding_key")
add_test("memcached_exist")
add_test("memcached_generate_hash")
add_test("memcached_haldenbrand_nblock_tcp_ndelay")
add_test("memcached_inc_dec")
add_test("memcached_ketama_compat")
add_test("memcached_noreply")
add_test("memcached_prepend")
add_test("memcached_regression_binary_block_add")
add_test("memcached_servers")
add_test("memcached_simple")
add_test("memcached_util")

add_test("memcat")
add_test("memcp")
add_test("memdump")
add_test("memerror")
add_test("memexist")
add_test("memflush")
