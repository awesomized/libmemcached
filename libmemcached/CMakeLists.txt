
find_package(FLEX)
find_package(BISON)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/csl)
bison_target(CSL_PARSER csl/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/csl/parser.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/csl/parser.h
        )
flex_target(CSL_SCANNER csl/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/csl/scanner.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/csl/scanner.h
        )
add_flex_bison_dependency(CSL_SCANNER CSL_PARSER)

set(LIBMEMCACHED_SOURCES
        csl/context.cc
        ${BISON_CSL_PARSER_OUTPUTS}
        ${FLEX_CSL_SCANNER_OUTPUTS}
        allocators.cc
        analyze.cc
        array.c
        auto.cc
        backtrace.cc
        behavior.cc
        byteorder.cc
        callback.cc
        connect.cc
        delete.cc
        do.cc
        dump.cc
        encoding_key.cc
        error.cc
        exist.cc
        fetch.cc
        flag.cc
        flush.cc
        flush_buffers.cc
        get.cc
        hash.cc
        hosts.cc
        initialize_query.cc
        instance.cc
        io.cc
        key.cc
        memcached.cc
        namespace.cc
        options.cc
        parse.cc
        poll.cc
        purge.cc
        quit.cc
        response.cc
        result.cc
        sasl.cc
        server.cc
        server_list.cc
        stats.cc
        storage.cc
        strerror.cc
        string.cc
        touch.cc
        udp.cc
        verbosity.cc
        version.cc
        virtual_bucket.c)

add_library(libmemcached SHARED
        ${LIBMEMCACHED_SOURCES})
add_library(memcached ALIAS libmemcached)
set_target_properties(libmemcached PROPERTIES LIBRARY_OUTPUT_NAME memcached)
target_link_libraries(libmemcached libhashkit Threads::Threads ${LIBSASL_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(libmemcached PRIVATE .. ${LIBSASL_INCLUDEDIR})
target_compile_definitions(libmemcached PRIVATE -DBUILDING_LIBMEMCACHED)

set_target_properties(libmemcached PROPERTIES SOVERSION ${LIBMEMCACHED_SO_VERSION})
install(TARGETS libmemcached EXPORT libmemcached
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcached)
install(EXPORT libmemcached DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING REGEX "(memcached|util)\\.h(pp)?"
        PATTERN csl EXCLUDE
        PATTERN memcached EXCLUDE
        PATTERN util EXCLUDE
        )

# FIXME: dtrace

add_library(libmemcachedinternal STATIC
        ${LIBMEMCACHED_SOURCES})
add_library(memcachedinternal ALIAS libmemcachedinternal)
set_target_properties(libmemcachedinternal PROPERTIES LIBRARY_OUTPUT_NAME memcachedinternal)
target_link_libraries(libmemcachedinternal libhashkit Threads::Threads ${LIBSASL_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(libmemcachedinternal PRIVATE .. ${LIBSASL_INCLUDEDIR})
target_compile_definitions(libmemcachedinternal PRIVATE -DBUILDING_LIBMEMCACHEDINTERNAL)
