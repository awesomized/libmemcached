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

install(TARGETS p9y EXPORT p9y-targets
        RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ${TARGET_NAMELINK_COMPONENT}
        )
export(EXPORT p9y-targets NAMESPACE libmemcached::)
install(EXPORT p9y-targets NAMESPACE libmemcached::
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

if(NOT HAVE_GETOPT_H)
    target_sources(p9y PRIVATE getopt.c)
endif()
