
find_package(FLEX)
find_package(BISON 2.3)

if(${BISON_VERSION} VERSION_GREATER_EQUAL 3.0)
        set(BISON_WARNINGS -Wno-deprecated)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/csl)
bison_target(CSL_PARSER csl/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/csl/parser.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/csl/parser.h
        COMPILE_FLAGS ${BISON_WARNINGS}
        )
flex_target(CSL_SCANNER csl/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/csl/scanner.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/csl/scanner.h
        )
add_flex_bison_dependency(CSL_SCANNER CSL_PARSER)

if(HAVE_DTRACE)
        add_custom_command(
                OUTPUT dtrace_probes.h
                COMMAND ${DTRACE_EXECUTABLE}
                ARGS -h -o dtrace_probes.h -s ${CMAKE_CURRENT_SOURCE_DIR}/libmemcached_probes.d
                MAIN_DEPENDENCY libmemcached_probes.d
        )
        add_custom_command(
                OUTPUT libmemcached_probes.o
                COMMAND ${DTRACE_EXECUTABLE}
                ARGS -o libmemcached_probes.o -G -s ${CMAKE_CURRENT_SOURCE_DIR}/libmemcached_probes.d
                MAIN_DEPENDENCY dtrace_probes.h
        )
        set(DTRACE_GENERATED_OUTPUT libmemcached_probes.o)
endif()

set(LIBMEMCACHED_SOURCES
        csl/context.cc
        ${BISON_CSL_PARSER_OUTPUTS}
        ${FLEX_CSL_SCANNER_OUTPUTS}
        ${DTRACE_GENERATED_OUTPUT}
        allocators.cc
        analyze.cc
        array.c
        auto.cc
        backtrace.cc
        behavior.cc
        byteorder.cc
        callback.cc
        common.h
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
set_target_properties(libmemcached PROPERTIES
        LIBRARY_OUTPUT_NAME memcached
        INSTALL_RPATH $ORIGIN
        SOVERSION ${LIBMEMCACHED_SO_VERSION}
        VERSION v${LIBMEMCACHED_VERSION})
target_compile_definitions(libmemcached PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcached PUBLIC libhashkit Threads::Threads ${LIBSASL_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(libmemcached PRIVATE ${LIBSASL_INCLUDEDIR})
target_include_directories(libmemcached PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcached PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

install(TARGETS libmemcached EXPORT libmemcached
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libmemcached)
install(EXPORT libmemcached DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)

install_public_headers(
        libmemcached

        memcached.h
        memcached.hpp
        util.h
        )

add_library(libmemcachedinternal STATIC
        ${LIBMEMCACHED_SOURCES})
add_library(memcachedinternal ALIAS libmemcachedinternal)
set_target_properties(libmemcachedinternal PROPERTIES LIBRARY_OUTPUT_NAME memcachedinternal)
target_compile_definitions(libmemcachedinternal PRIVATE -DBUILDING_LIBMEMCACHEDINTERNAL)
target_link_libraries(libmemcachedinternal PUBLIC libhashkit Threads::Threads ${LIBSASL_LIBRARIES} ${CMAKE_DL_LIBS})
target_include_directories(libmemcachedinternal PRIVATE ${LIBSASL_INCLUDEDIR})
target_include_directories(libmemcachedinternal PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedinternal PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

# let libmemcached depend on libmemcachedinternal to ensure that they do not
# compete for the same generated dependencies
add_dependencies(libmemcached libmemcachedinternal)
