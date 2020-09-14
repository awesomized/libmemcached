
file(GLOB_RECURSE TESTING_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
set(TESTING_ROOT ${CMAKE_CURRENT_BINARY_DIR})
configure_file(conf.h.in conf.h @ONLY)
add_executable(catch_main ${TESTING_SRC})
set_target_properties(catch_main PROPERTIES CXX_STANDARD 17)
target_include_directories(catch_main PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src)
target_link_libraries(catch_main PRIVATE libhashkit libmemcached libmemcachedutil)
add_dependencies(catch_main ${CLIENTS})

add_test("lib/Server" catch_main "lib/Server")
add_test("lib/Cluster" catch_main "lib/Cluster")

add_test("hashkit" catch_main "hashkit")

add_test("memcached_append" catch_main "memcached_append")
add_test("memcached_basic" catch_main "memcached_basic")
add_test("memcached_callbacks" catch_main "memcached_callbacks")
add_test("memcached_cas" catch_main "memcached_cas")
add_test("memcached_dump" catch_main "memcached_dump")
add_test("memcached_encoding_key" catch_main "memcached_encoding_key")
add_test("memcached_exist" catch_main "memcached_exist")
add_test("memcached_generate_hash" catch_main "memcached_generate_hash")
add_test("memcached_haldenbrand_nblock_tcp_ndelay" catch_main "memcached_haldenbrand_nblock_tcp_ndelay")
add_test("memcached_inc_dec" catch_main "memcached_inc_dec")
add_test("memcached_ketama_compat" catch_main "memcached_ketama_compat")
add_test("memcached_noreply" catch_main "memcached_noreply")
add_test("memcached_prepend" catch_main "memcached_prepend")
add_test("memcached_regression_binary_block_add" catch_main "memcached_regression_binary_block_add")
add_test("memcached_servers" catch_main "memcached_servers")
add_test("memcached_simple" catch_main "memcached_simple")
add_test("memcached_util" catch_main "memcached_util")

add_test("memcat" catch_main "memcat")
add_test("memcp" catch_main "memcp")
add_test("memdump" catch_main "memdump")
add_test("memerror" catch_main "memerror")
add_test("memexist" catch_main "memexist")
