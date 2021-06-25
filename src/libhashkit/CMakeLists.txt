set(libhashkit_sources
        aes.cc
        algorithm.cc
        common.h
        crc32.cc
        digest.cc
        encrypt.cc
        fnv_32.cc
        fnv_64.cc
        function.cc
        has.cc
        hashkit.cc
        hsieh.cc
        jenkins.cc
        md5.cc
        murmur.cc
        murmur3.cc
        murmur3_api.cc
        one_at_a_time.cc
        rijndael.cc
        str_algorithm.cc
        strerror.cc
        string.cc
        )
add_library(libhashkit SHARED)
add_library(hashkit ALIAS libhashkit)
set_target_properties(libhashkit PROPERTIES
        CXX_STANDARD ${CXX_STANDARD}
        LIBRARY_OUTPUT_NAME hashkit
        LIBRARY_OUTPUT_NAME_DEBUG hashkit-dbg
        SOVERSION ${LIBHASHKIT_SO_SOVERSION}
        VERSION ${LIBHASHKIT_SO_VERSION})
target_compile_options(libhashkit PRIVATE -DBUILDING_HASHKIT)
target_include_directories(libhashkit PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libhashkit PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(OpenSSL)
if(NOT OPENSSL_FOUND)
  message(WARNING "crypto library not found")
else()
  add_compile_definitions(WITH_OPENSSL)
  target_link_libraries(libhashkit PUBLIC OpenSSL::Crypto)
endif()

configure_file(hashkitcon.h.in hashkitcon.h @ONLY)

install(TARGETS libhashkit EXPORT libhashkit-targets
        RUNTIME COMPONENT bin DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY COMPONENT lib DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ${TARGET_NAMELINK_COMPONENT}
        )
export(EXPORT libhashkit-targets NAMESPACE libmemcached::)
install(EXPORT libhashkit-targets NAMESPACE libmemcached::
        COMPONENT dev
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install_public_headers(
        libhashkit

        hashkit.h
        )

target_sources(libhashkit PRIVATE
        ${libhashkit_sources}
        ${libhashkit_includes})
