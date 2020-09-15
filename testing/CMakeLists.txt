if(NOT BUILD_TESTING OR NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    return()
endif()

include(CMake/Catch.cmake)

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

catch_discover_tests(runtests
        TEST_SPEC "lib*")
catch_discover_tests(runtests
        TEST_SPEC "hashkit*")
catch_discover_tests(runtests
        TEST_SPEC "memcached*")
catch_discover_tests(runtests
        TEST_SPEC "bin/*")
