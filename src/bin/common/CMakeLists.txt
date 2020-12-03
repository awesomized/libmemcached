add_library(libclient_common STATIC options.cpp checks.hpp random.hpp time.hpp)
add_library(client_common ALIAS libclient_common)
set_target_properties(libclient_common PROPERTIES CXX_STANDARD ${CXX_STANDARD})
target_link_libraries(libclient_common PRIVATE p9y)
target_link_libraries(libclient_common PUBLIC libmemcached)
target_include_directories(libclient_common PUBLIC
        .
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
