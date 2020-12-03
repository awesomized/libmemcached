add_library(p9y STATIC
    getopt.hpp
    libgen.hpp
    realpath.hpp
    socket.hpp
    poll.hpp
    random.hpp
    index.hpp
    p9y.cpp
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
if(NOT HAVE_LIBGEN_H)
    target_sources(p9y PRIVATE libgen.c)
endif()
if(NOT HAVE_REALPATH)
    target_sources(p9y PRIVATE realpath.c)
endif()
if(WIN32)
    target_sources(p9y PRIVATE socket.c)
endif()
if(NOT HAVE_POLL_H AND NOT HAVE_SYS_POLL_H AND NOT WIN32)
    target_sources(p9y PRIVATE poll.c)
endif()
if(NOT HAVE_GETTIMEOFDAY)
    target_sources(p9y PRIVATE gettimeofday.cpp)
endif()
