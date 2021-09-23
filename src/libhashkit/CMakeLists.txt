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

if(ENABLE_OPENSSL_CRYPTO)
        find_package(OpenSSL)
        if(OPENSSL_FOUND)
                if(OPENSSL_CRYPTO_LIBRARY)
                        target_compile_definitions(libhashkit PRIVATE HAVE_OPENSSL_CRYPTO)
                        target_link_libraries(libhashkit PUBLIC OpenSSL::Crypto)
                        pkgconfig_export(REQUIRES_PRIVATE libcrypto)
                else()
                        message(WARNING "Could not find OpenSSL::Crypto")
                endif()
        endif()
endif()

if (NOT OPENSSL_CRYPTO_LIBRARY)
        set (libhashkit_sources ${libhashkit_sources} rijndael.cc)
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
