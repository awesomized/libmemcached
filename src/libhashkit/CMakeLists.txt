add_library(libhashkit SHARED
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
add_library(hashkit ALIAS libhashkit)
set_target_properties(libhashkit PROPERTIES
        CXX_STANDARD ${CXX_STANDARD}
        LIBRARY_OUTPUT_NAME hashkit
        LIBRARY_OUTPUT_NAME_DEBUG hashkit-dbg
        SOVERSION ${LIBHASHKIT_SO_VERSION})
target_compile_options(libhashkit PRIVATE -DBUILDING_HASHKIT)
target_include_directories(libhashkit PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
        ${CMAKE_BINARY_DIR})
target_include_directories(libhashkit PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)
configure_file(hashkitcon.h.in hashkitcon.h @ONLY)

install(TARGETS libhashkit EXPORT libhashkit-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
export(EXPORT libhashkit-targets NAMESPACE libmemcached::)
install(EXPORT libhashkit-targets NAMESPACE libmemcached:: DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake)

install_public_headers(
        libhashkit

        hashkit.h
        )
