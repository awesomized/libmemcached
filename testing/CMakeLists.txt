
file(GLOB_RECURSE TESTING_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_executable(catch_main ${TESTING_SRC})
set_target_properties(catch_main PROPERTIES CXX_STANDARD 17)
target_link_libraries(catch_main libhashkit libmemcached libmemcachedutil)
