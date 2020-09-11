
file(GLOB_RECURSE TESTING_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
set(TESTING_ROOT ${CMAKE_CURRENT_BINARY_DIR})
configure_file(conf.h.in conf.h @ONLY)
add_executable(catch_main ${TESTING_SRC})
set_target_properties(catch_main PROPERTIES CXX_STANDARD 17)
target_link_libraries(catch_main libhashkit libmemcached libmemcachedutil)
add_dependencies(catch_main ${CLIENTS})
