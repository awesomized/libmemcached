add_library(p9y STATIC
    getopt.hpp
    libgen.hpp
    realpath.hpp
    socket.hpp
    poll.hpp
    random.hpp
    index.hpp
    clock_gettime.hpp
    p9y.cpp
    )

set_target_properties(p9y PROPERTIES
    CXX_STANDARD ${CXX_STANDARD}
    )
target_include_directories(p9y PRIVATE
    .
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/include
    )

if(NOT HAVE_GETOPT_H)
    target_sources(p9y PRIVATE getopt.c)
endif()
