find_package(FLEX)
find_package(BISON 2.3)

if("${BISON_VERSION}" VERSION_GREATER_EQUAL 3.0)
    set(BISON_WARNINGS -Wno-deprecated)
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/csl)
bison_target(CSL_PARSER csl/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/csl/parser.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/csl/parser.h
        COMPILE_FLAGS ${BISON_WARNINGS}
        )
set_source_files_properties(${BISON_CSL_PARSER_OUTPUTS} PROPERTIES
        SKIP_UNITY_BUILD_INCLUSION ON)
if(NOT HAVE_UNISTD_H)
    set(FLEX_FLAGS --nounistd)
endif()
flex_target(CSL_SCANNER csl/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/csl/scanner.cc
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/csl/scanner.h
        COMPILE_FLAGS "${FLEX_FLAGS}"
        )
set_source_files_properties(${FLEX_CSL_SCANNER_OUTPUTS} PROPERTIES
        SKIP_UNITY_BUILD_INCLUSION ON)
add_flex_bison_dependency(CSL_SCANNER CSL_PARSER)

set(libmemcached_sources
        csl/context.cc
        ${FLEX_CSL_SCANNER_OUTPUTS}
        ${BISON_CSL_PARSER_OUTPUTS}
        allocators.cc
        analyze.cc
        array.cc
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
        virtual_bucket.cc)

check_cxx_compiler_flag(-Wno-deprecated-register W_NO_DEPRECATED_REGISTER)

if(W_NO_DEPRECATED_REGISTER)
        set_source_files_properties(
                ${FLEX_CSL_SCANNER_OUTPUTS}
                ${BISON_CSL_PARSER_OUTPUTS}
                PROPERTIES COMPILE_OPTIONS -Wno-deprecated-register
        )
endif()

add_library(libmemcached SHARED)
add_library(memcached ALIAS libmemcached)
set_target_properties(libmemcached PROPERTIES
        CXX_STANDARD ${CXX_STANDARD}
        LIBRARY_OUTPUT_NAME memcached
        LIBRARY_OUTPUT_NAME_DEBUG memcached-dbg
        SOVERSION ${LIBMEMCACHED_SO_SOVERSION}
        VERSION ${LIBMEMCACHED_SO_VERSION})
target_compile_definitions(libmemcached PRIVATE -DBUILDING_LIBMEMCACHED)
target_link_libraries(libmemcached PUBLIC libhashkit Threads::Threads ${CMAKE_DL_LIBS})
target_link_libraries(libmemcached PRIVATE p9y)
if(MSVC)
    target_link_libraries(libmemcached PUBLIC wsock32 ws2_32)
endif()
if(HAVE_BACKTRACE)
    target_link_libraries(libmemcached PUBLIC ${BACKTRACE})
endif()
if(HAVE_LIBSASL)
    target_link_libraries(libmemcached PUBLIC ${LIBSASL})
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "SunPro")
    # see https://docs.oracle.com/cd/E77782_01/html/E77789/bkamq.html#OSSCPgrxeu
    target_link_libraries(libmemcached INTERFACE stdc++ gcc_s CrunG3 c)
endif()
target_include_directories(libmemcached PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcached PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

enable_dtrace_for(libmemcached libmemcached_probes.d dtrace_probes.h)

install(TARGETS libmemcached EXPORT libmemcached-targets
        RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ${TARGET_NAMELINK_COMPONENT}
        )
export(EXPORT libmemcached-targets NAMESPACE libmemcached::)
install(EXPORT libmemcached-targets NAMESPACE libmemcached::
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install_public_headers(
        libmemcached

        memcached.h
        memcached.hpp
        util.h
        )

target_sources(libmemcached PRIVATE
        ${libmemcached_sources}
        ${libmemcached_includes})

# TODO: do we really need this?

add_library(libmemcachedinternal STATIC)
add_library(memcachedinternal ALIAS libmemcachedinternal)
set_target_properties(libmemcachedinternal PROPERTIES CXX_STANDARD ${CXX_STANDARD} LIBRARY_OUTPUT_NAME memcachedinternal)
target_compile_definitions(libmemcachedinternal PRIVATE -DBUILDING_LIBMEMCACHEDINTERNAL)
target_link_libraries(libmemcachedinternal PUBLIC libhashkit Threads::Threads ${CMAKE_DL_LIBS})
target_link_libraries(libmemcachedinternal PRIVATE p9y)
if(MSVC OR MINGW)
    target_link_libraries(libmemcached PUBLIC wsock32 ws2_32)
endif()
if(HAVE_LIBSASL)
    target_link_libraries(libmemcachedinternal PUBLIC ${LIBSASL})
endif()
if(HAVE_BACKTRACE)
    target_link_libraries(libmemcachedinternal PUBLIC ${BACKTRACE})
endif()
target_include_directories(libmemcachedinternal PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libmemcachedinternal PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

enable_dtrace_for(libmemcachedinternal libmemcached_probes.d dtrace_probes.h)

# let libmemcached depend on libmemcachedinternal to ensure that they do not
# compete for the same generated dependencies
add_dependencies(libmemcached libmemcachedinternal)

target_sources(libmemcachedinternal PRIVATE
        ${libmemcached_sources}
        ${libmemcached_includes})
