add_library(libclient_common STATIC utilities.cc generator.cc execute.cc)
add_library(client_common ALIAS libclient_common)
target_link_libraries(libclient_common PRIVATE libmemcachedinternal)
target_include_directories(libclient_common PUBLIC
        .
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
